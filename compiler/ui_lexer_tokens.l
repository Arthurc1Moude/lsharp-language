/* ============================================================================
 * L# UI LEXER TOKENS - PREMIUM UI SYSTEM
 * Additional tokens for advanced UI development
 * ============================================================================ */

%{
#include "parser.tab.h"
#include <string.h>
%}

%%

/* ============================================================================
 * UI COMPONENT KEYWORDS
 * ============================================================================ */
"component"         { return COMPONENT; }
"render"            { return RENDER; }
"props"             { return PROPS; }
"state"             { return STATE; }
"computed"          { return COMPUTED; }

/* ============================================================================
 * REACT-LIKE HOOKS
 * ============================================================================ */
"useState"          { return USE_STATE; }
"useEffect"         { return USE_EFFECT; }
"useContext"        { return USE_CONTEXT; }
"useRef"            { return USE_REF; }
"useMemo"           { return USE_MEMO; }
"useCallback"       { return USE_CALLBACK; }
"useReducer"        { return USE_REDUCER; }
"useLayoutEffect"   { return USE_LAYOUT_EFFECT; }
"useImperativeHandle" { return USE_IMPERATIVE_HANDLE; }
"useDebugValue"     { return USE_DEBUG_VALUE; }

/* ============================================================================
 * LIFECYCLE METHODS
 * ============================================================================ */
"onMount"           { return ON_MOUNT; }
"onUnmount"         { return ON_UNMOUNT; }
"onUpdate"          { return ON_UPDATE; }
"beforeRender"      { return BEFORE_RENDER; }
"afterRender"       { return AFTER_RENDER; }
"shouldUpdate"      { return SHOULD_UPDATE; }
"onError"           { return ON_ERROR; }

/* ============================================================================
 * LAYOUT KEYWORDS
 * ============================================================================ */
"layout"            { return LAYOUT; }
"flex"              { return FLEX; }
"grid"              { return GRID; }
"stack"             { return STACK; }
"absolute"          { return ABSOLUTE; }
"relative"          { return RELATIVE; }
"fixed"             { return FIXED; }
"sticky"            { return STICKY; }

/* Flexbox Properties */
"flexDirection"     { return FLEX_DIRECTION; }
"flexWrap"          { return FLEX_WRAP; }
"flexGrow"          { return FLEX_GROW; }
"flexShrink"        { return FLEX_SHRINK; }
"flexBasis"         { return FLEX_BASIS; }
"alignItems"        { return ALIGN_ITEMS; }
"alignContent"      { return ALIGN_CONTENT; }
"alignSelf"         { return ALIGN_SELF; }
"justifyContent"    { return JUSTIFY_CONTENT; }
"justifyItems"      { return JUSTIFY_ITEMS; }
"justifySelf"       { return JUSTIFY_SELF; }

/* Flexbox Values */
"row"               { return ROW; }
"column"            { return COLUMN; }
"row-reverse"       { return ROW_REVERSE; }
"column-reverse"    { return COLUMN_REVERSE; }
"wrap"              { return WRAP; }
"nowrap"            { return NOWRAP; }
"center"            { return CENTER; }
"start"             { return START; }
"end"               { return END; }
"stretch"           { return STRETCH; }
"baseline"          { return BASELINE; }
"space-between"     { return SPACE_BETWEEN; }
"space-around"      { return SPACE_AROUND; }
"space-evenly"      { return SPACE_EVENLY; }

/* Grid Properties */
"gridTemplateColumns" { return GRID_TEMPLATE_COLUMNS; }
"gridTemplateRows"    { return GRID_TEMPLATE_ROWS; }
"gridTemplateAreas"   { return GRID_TEMPLATE_AREAS; }
"gridColumn"          { return GRID_COLUMN; }
"gridRow"             { return GRID_ROW; }
"gridArea"            { return GRID_AREA; }
"gridGap"             { return GRID_GAP; }
"gridColumnGap"       { return GRID_COLUMN_GAP; }
"gridRowGap"          { return GRID_ROW_GAP; }
"gridAutoFlow"        { return GRID_AUTO_FLOW; }
"gridAutoColumns"     { return GRID_AUTO_COLUMNS; }
"gridAutoRows"        { return GRID_AUTO_ROWS; }

/* Grid Functions */
"repeat"            { return REPEAT; }
"minmax"            { return MINMAX; }
"auto"              { return AUTO; }
"fr"                { return FR; }
"fit-content"       { return FIT_CONTENT; }

/* ============================================================================
 * STYLE KEYWORDS
 * ============================================================================ */
"style"             { return STYLE; }
"className"         { return CLASS_NAME; }
"id"                { return ID; }

/* Color & Background */
"color"             { return COLOR; }
"background"        { return BACKGROUND; }
"backgroundColor"   { return BACKGROUND_COLOR; }
"backgroundImage"   { return BACKGROUND_IMAGE; }
"backgroundSize"    { return BACKGROUND_SIZE; }
"backgroundPosition" { return BACKGROUND_POSITION; }
"backgroundRepeat"  { return BACKGROUND_REPEAT; }
"opacity"           { return OPACITY; }

/* Border */
"border"            { return BORDER; }
"borderRadius"      { return BORDER_RADIUS; }
"borderWidth"       { return BORDER_WIDTH; }
"borderColor"       { return BORDER_COLOR; }
"borderStyle"       { return BORDER_STYLE; }
"borderTop"         { return BORDER_TOP; }
"borderRight"       { return BORDER_RIGHT; }
"borderBottom"      { return BORDER_BOTTOM; }
"borderLeft"        { return BORDER_LEFT; }

/* Spacing */
"margin"            { return MARGIN; }
"marginTop"         { return MARGIN_TOP; }
"marginRight"       { return MARGIN_RIGHT; }
"marginBottom"      { return MARGIN_BOTTOM; }
"marginLeft"        { return MARGIN_LEFT; }
"padding"           { return PADDING; }
"paddingTop"        { return PADDING_TOP; }
"paddingRight"      { return PADDING_RIGHT; }
"paddingBottom"     { return PADDING_BOTTOM; }
"paddingLeft"       { return PADDING_LEFT; }
"gap"               { return GAP; }

/* Sizing */
"width"             { return WIDTH; }
"height"            { return HEIGHT; }
"minWidth"          { return MIN_WIDTH; }
"maxWidth"          { return MAX_WIDTH; }
"minHeight"         { return MIN_HEIGHT; }
"maxHeight"         { return MAX_HEIGHT; }

/* Typography */
"fontSize"          { return FONT_SIZE; }
"fontWeight"        { return FONT_WEIGHT; }
"fontFamily"        { return FONT_FAMILY; }
"fontStyle"         { return FONT_STYLE; }
"lineHeight"        { return LINE_HEIGHT; }
"letterSpacing"     { return LETTER_SPACING; }
"textAlign"         { return TEXT_ALIGN; }
"textDecoration"    { return TEXT_DECORATION; }
"textTransform"     { return TEXT_TRANSFORM; }
"whiteSpace"        { return WHITE_SPACE; }
"wordBreak"         { return WORD_BREAK; }
"overflow"          { return OVERFLOW; }
"textOverflow"      { return TEXT_OVERFLOW; }

/* ============================================================================
 * GRADIENT KEYWORDS
 * ============================================================================ */
"linearGradient"    { return LINEAR_GRADIENT; }
"radialGradient"    { return RADIAL_GRADIENT; }
"conicGradient"     { return CONIC_GRADIENT; }
"to"                { return TO; }
"from"              { return FROM; }
"deg"               { return DEG; }
"rad"               { return RAD; }
"turn"              { return TURN; }

/* ============================================================================
 * TRANSFORM KEYWORDS
 * ============================================================================ */
"transform"         { return TRANSFORM; }
"translate"         { return TRANSLATE; }
"translateX"        { return TRANSLATE_X; }
"translateY"        { return TRANSLATE_Y; }
"translateZ"        { return TRANSLATE_Z; }
"translate3d"       { return TRANSLATE_3D; }
"scale"             { return SCALE; }
"scaleX"            { return SCALE_X; }
"scaleY"            { return SCALE_Y; }
"scaleZ"            { return SCALE_Z; }
"scale3d"           { return SCALE_3D; }
"rotate"            { return ROTATE; }
"rotateX"           { return ROTATE_X; }
"rotateY"           { return ROTATE_Y; }
"rotateZ"           { return ROTATE_Z; }
"rotate3d"          { return ROTATE_3D; }
"skew"              { return SKEW; }
"skewX"             { return SKEW_X; }
"skewY"             { return SKEW_Y; }
"matrix"            { return MATRIX; }
"matrix3d"          { return MATRIX_3D; }
"perspective"       { return PERSPECTIVE; }
"transformOrigin"   { return TRANSFORM_ORIGIN; }
"transformStyle"    { return TRANSFORM_STYLE; }

/* ============================================================================
 * FILTER KEYWORDS
 * ============================================================================ */
"filter"            { return FILTER; }
"blur"              { return BLUR; }
"brightness"        { return BRIGHTNESS; }
"contrast"          { return CONTRAST; }
"grayscale"         { return GRAYSCALE; }
"hueRotate"         { return HUE_ROTATE; }
"invert"            { return INVERT; }
"saturate"          { return SATURATE; }
"sepia"             { return SEPIA; }
"dropShadow"        { return DROP_SHADOW; }
"backdropFilter"    { return BACKDROP_FILTER; }

/* ============================================================================
 * ANIMATION KEYWORDS
 * ============================================================================ */
"animation"         { return ANIMATION; }
"keyframes"         { return KEYFRAMES; }
"animationName"     { return ANIMATION_NAME; }
"animationDuration" { return ANIMATION_DURATION; }
"animationDelay"    { return ANIMATION_DELAY; }
"animationTimingFunction" { return ANIMATION_TIMING_FUNCTION; }
"animationIterationCount" { return ANIMATION_ITERATION_COUNT; }
"animationDirection" { return ANIMATION_DIRECTION; }
"animationFillMode" { return ANIMATION_FILL_MODE; }
"animationPlayState" { return ANIMATION_PLAY_STATE; }

/* Animation Values */
"ease"              { return EASE; }
"ease-in"           { return EASE_IN; }
"ease-out"          { return EASE_OUT; }
"ease-in-out"       { return EASE_IN_OUT; }
"linear"            { return LINEAR; }
"step-start"        { return STEP_START; }
"step-end"          { return STEP_END; }
"infinite"          { return INFINITE; }
"normal"            { return NORMAL; }
"reverse"           { return REVERSE; }
"alternate"         { return ALTERNATE; }
"alternate-reverse" { return ALTERNATE_REVERSE; }
"forwards"          { return FORWARDS; }
"backwards"         { return BACKWARDS; }
"both"              { return BOTH; }
"running"           { return RUNNING; }
"paused"            { return PAUSED; }

/* ============================================================================
 * TRANSITION KEYWORDS
 * ============================================================================ */
"transition"        { return TRANSITION; }
"transitionProperty" { return TRANSITION_PROPERTY; }
"transitionDuration" { return TRANSITION_DURATION; }
"transitionDelay"   { return TRANSITION_DELAY; }
"transitionTimingFunction" { return TRANSITION_TIMING_FUNCTION; }
"all"               { return ALL; }

/* ============================================================================
 * SHADOW KEYWORDS
 * ============================================================================ */
"boxShadow"         { return BOX_SHADOW; }
"textShadow"        { return TEXT_SHADOW; }
"inset"             { return INSET; }

/* ============================================================================
 * EVENT KEYWORDS
 * ============================================================================ */
"on"                { return ON; }
"onClick"           { return ON_CLICK; }
"onChange"          { return ON_CHANGE; }
"onSubmit"          { return ON_SUBMIT; }
"onInput"           { return ON_INPUT; }
"onFocus"           { return ON_FOCUS; }
"onBlur"            { return ON_BLUR; }
"onKeyDown"         { return ON_KEY_DOWN; }
"onKeyUp"           { return ON_KEY_UP; }
"onKeyPress"        { return ON_KEY_PRESS; }
"onMouseDown"       { return ON_MOUSE_DOWN; }
"onMouseUp"         { return ON_MOUSE_UP; }
"onMouseMove"       { return ON_MOUSE_MOVE; }
"onMouseEnter"      { return ON_MOUSE_ENTER; }
"onMouseLeave"      { return ON_MOUSE_LEAVE; }
"onMouseOver"       { return ON_MOUSE_OVER; }
"onMouseOut"        { return ON_MOUSE_OUT; }
"onWheel"           { return ON_WHEEL; }
"onScroll"          { return ON_SCROLL; }
"onDrag"            { return ON_DRAG; }
"onDragStart"       { return ON_DRAG_START; }
"onDragEnd"         { return ON_DRAG_END; }
"onDragEnter"       { return ON_DRAG_ENTER; }
"onDragLeave"       { return ON_DRAG_LEAVE; }
"onDragOver"        { return ON_DRAG_OVER; }
"onDrop"            { return ON_DROP; }
"onTouchStart"      { return ON_TOUCH_START; }
"onTouchMove"       { return ON_TOUCH_MOVE; }
"onTouchEnd"        { return ON_TOUCH_END; }
"onTouchCancel"     { return ON_TOUCH_CANCEL; }

/* ============================================================================
 * THEME KEYWORDS
 * ============================================================================ */
"theme"             { return THEME; }
"darkMode"          { return DARK_MODE; }
"lightMode"         { return LIGHT_MODE; }
"palette"           { return PALETTE; }
"primary"           { return PRIMARY; }
"secondary"         { return SECONDARY; }
"accent"            { return ACCENT; }
"success"           { return SUCCESS; }
"warning"           { return WARNING; }
"error"             { return ERROR; }
"info"              { return INFO; }

/* ============================================================================
 * RESPONSIVE KEYWORDS
 * ============================================================================ */
"@media"            { return AT_MEDIA; }
"breakpoint"        { return BREAKPOINT; }
"mobile"            { return MOBILE; }
"tablet"            { return TABLET; }
"desktop"           { return DESKTOP; }
"xs"                { return XS; }
"sm"                { return SM; }
"md"                { return MD; }
"lg"                { return LG; }
"xl"                { return XL; }
"xxl"               { return XXL; }

/* ============================================================================
 * ACCESSIBILITY KEYWORDS
 * ============================================================================ */
"aria-label"        { return ARIA_LABEL; }
"aria-describedby"  { return ARIA_DESCRIBEDBY; }
"aria-hidden"       { return ARIA_HIDDEN; }
"aria-live"         { return ARIA_LIVE; }
"aria-expanded"     { return ARIA_EXPANDED; }
"aria-selected"     { return ARIA_SELECTED; }
"aria-checked"      { return ARIA_CHECKED; }
"aria-disabled"     { return ARIA_DISABLED; }
"role"              { return ROLE; }
"tabIndex"          { return TABINDEX; }
"alt"               { return ALT; }
"title"             { return TITLE; }

/* ============================================================================
 * CONTEXT & PORTAL KEYWORDS
 * ============================================================================ */
"createContext"     { return CREATE_CONTEXT; }
"Provider"          { return PROVIDER; }
"Consumer"          { return CONSUMER; }
"createPortal"      { return CREATE_PORTAL; }

/* ============================================================================
 * SPECIAL UI KEYWORDS
 * ============================================================================ */
"ref"               { return REF; }
"key"               { return KEY; }
"children"          { return CHILDREN; }
"defaultValue"      { return DEFAULT_VALUE; }
"value"             { return VALUE; }
"checked"           { return CHECKED; }
"disabled"          { return DISABLED; }
"readOnly"          { return READ_ONLY; }
"placeholder"       { return PLACEHOLDER; }
"autoFocus"         { return AUTO_FOCUS; }
"required"          { return REQUIRED; }
"pattern"           { return PATTERN; }
"min"               { return MIN; }
"max"               { return MAX; }
"step"              { return STEP; }
"multiple"          { return MULTIPLE; }
"accept"            { return ACCEPT; }
"src"               { return SRC; }
"href"              { return HREF; }
"target"            { return TARGET; }
"download"          { return DOWNLOAD; }
"type"              { return TYPE; }
"name"              { return NAME; }

/* ============================================================================
 * CURSOR & POINTER KEYWORDS
 * ============================================================================ */
"cursor"            { return CURSOR; }
"pointer"           { return POINTER; }
"grab"              { return GRAB; }
"grabbing"          { return GRABBING; }
"move"              { return MOVE; }
"not-allowed"       { return NOT_ALLOWED; }
"wait"              { return WAIT; }
"text"              { return TEXT; }
"crosshair"         { return CROSSHAIR; }
"help"              { return HELP; }
"zoom-in"           { return ZOOM_IN; }
"zoom-out"          { return ZOOM_OUT; }

/* ============================================================================
 * Z-INDEX & STACKING
 * ============================================================================ */
"zIndex"            { return Z_INDEX; }
"isolation"         { return ISOLATION; }
"mixBlendMode"      { return MIX_BLEND_MODE; }

/* ============================================================================
 * CLIP & MASK
 * ============================================================================ */
"clipPath"          { return CLIP_PATH; }
"mask"              { return MASK; }
"maskImage"         { return MASK_IMAGE; }
"maskSize"          { return MASK_SIZE; }
"maskPosition"      { return MASK_POSITION; }

/* ============================================================================
 * SCROLL KEYWORDS
 * ============================================================================ */
"overflowX"         { return OVERFLOW_X; }
"overflowY"         { return OVERFLOW_Y; }
"scrollBehavior"    { return SCROLL_BEHAVIOR; }
"scrollSnapType"    { return SCROLL_SNAP_TYPE; }
"scrollSnapAlign"   { return SCROLL_SNAP_ALIGN; }
"smooth"            { return SMOOTH; }
"visible"           { return VISIBLE; }
"hidden"            { return HIDDEN; }
"scroll"            { return SCROLL; }

/* ============================================================================
 * DISPLAY KEYWORDS
 * ============================================================================ */
"display"           { return DISPLAY; }
"block"             { return BLOCK; }
"inline"            { return INLINE; }
"inline-block"      { return INLINE_BLOCK; }
"none"              { return NONE; }

/* ============================================================================
 * POSITION KEYWORDS
 * ============================================================================ */
"position"          { return POSITION; }
"top"               { return TOP; }
"right"             { return RIGHT; }
"bottom"            { return BOTTOM; }
"left"              { return LEFT; }

/* ============================================================================
 * UNITS
 * ============================================================================ */
"px"                { return PX; }
"em"                { return EM; }
"rem"               { return REM; }
"vh"                { return VH; }
"vw"                { return VW; }
"vmin"              { return VMIN; }
"vmax"              { return VMAX; }
"%"                 { return PERCENT; }
"ch"                { return CH; }
"ex"                { return EX; }

/* ============================================================================
 * JSX TOKENS
 * ============================================================================ */
"<"                 { return LT; }
">"                 { return GT; }
"</"                { return LT_SLASH; }
"/>"                { return SLASH_GT; }
"..."               { return SPREAD; }

/* ============================================================================
 * SPECIAL OPERATORS
 * ============================================================================ */
"@"                 { return AT; }
"#"                 { return HASH; }
"?"                 { return QUESTION; }
"??"                { return NULL_COALESCE; }
"?."                { return OPTIONAL_CHAIN; }
"=>"                { return FAT_ARROW; }
"->"                { return ARROW; }
"::"                { return DOUBLE_COLON; }
"..."               { return ELLIPSIS; }

%%

/* End of UI Lexer Tokens */
