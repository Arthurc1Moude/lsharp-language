/*
Language: L# (L-Sharp)
Description: Native UI programming language with React-like syntax
Author: L# Team
Website: https://lsharp-lang.org
Category: ui, native
*/

export default function(hljs) {
  const KEYWORDS = {
    keyword:
      'if else elif match case for while loop break continue return yield await async ' +
      'let const var fn func function class struct enum interface trait impl type alias ' +
      'pub priv protected  final abstract virtual override mut ref unsafe extern dyn macro derive ' +
      'new delete sizeof typeof instanceof as in is',
    built_in:
      'int float double bool string char byte void any never ' +
      'true false null nil undefined this self super base',
    literal:
      'true false null nil undefined'
  };

  const MODULE_KEYWORDS = {
    keyword: 'import export deport report from direct use as'
  };

  const UI_KEYWORDS = {
    keyword: 'component render props state computed',
    built_in:
      'Container Card Button Input Text Image Icon Canvas GLCanvas Grid Stack Box ' +
      'Modal Tooltip TextArea Checkbox Radio Switch Slider Dropdown'
  };

  const HOOKS = {
    built_in:
      'useState useEffect useContext useRef useMemo useCallback useReducer ' +
      'useLayoutEffect useImperativeHandle useDebugValue'
  };

  const LIFECYCLE = {
    built_in:
      'onMount onUnmount onUpdate beforeRender afterRender shouldUpdate onError'
  };

  const EVENTS = {
    built_in:
      'onClick onChange onSubmit onInput onFocus onBlur ' +
      'onKeyDown onKeyUp onKeyPress ' +
      'onMouseDown onMouseUp onMouseMove onMouseEnter onMouseLeave onMouseOver onMouseOut ' +
      'onWheel onScroll ' +
      'onDrag onDragStart onDragEnd onDragEnter onDragLeave onDragOver onDrop ' +
      'onTouchStart onTouchMove onTouchEnd onTouchCancel'
  };

  const STYLE_PROPS = {
    built_in:
      'style className width height margin padding border borderRadius ' +
      'background backgroundColor color fontSize fontWeight fontFamily ' +
      'display position top right bottom left zIndex opacity ' +
      'transform transition animation boxShadow textShadow overflow cursor'
  };

  const LAYOUT = {
    keyword: 'layout flex grid stack absolute relative fixed sticky',
    built_in:
      'flexDirection flexWrap flexGrow flexShrink flexBasis ' +
      'alignItems alignContent alignSelf justifyContent justifyItems justifySelf ' +
      'gridTemplateColumns gridTemplateRows gridTemplateAreas ' +
      'gridColumn gridRow gridArea gridGap gridColumnGap gridRowGap ' +
      'gridAutoFlow gridAutoColumns gridAutoRows ' +
      'row column wrap nowrap center start end stretch baseline ' +
      'space-between space-around space-evenly'
  };

  const FUNCTIONS = {
    built_in:
      'linearGradient radialGradient conicGradient ' +
      'translate translateX translateY scale scaleX scaleY rotate rotateX rotateY ' +
      'skew skewX skewY matrix perspective ' +
      'blur brightness contrast grayscale hueRotate invert saturate sepia dropShadow ' +
      'printf scanf malloc free memcpy strlen strcmp ' +
      'glBegin glEnd glVertex3f glColor3f'
  };

  const ANIMATION = {
    keyword: 'animation keyframes transition',
    built_in:
      'ease ease-in ease-out ease-in-out linear step-start step-end ' +
      'infinite normal reverse alternate alternate-reverse ' +
      'forwards backwards both running paused'
  };

  const COMMENT = hljs.COMMENT('//', '$');
  const BLOCK_COMMENT = hljs.COMMENT('/\\*', '\\*/');

  const STRING = {
    className: 'string',
    variants: [
      hljs.QUOTE_STRING_MODE,
      hljs.APOS_STRING_MODE,
      {
        begin: '`',
        end: '`',
        contains: [
          hljs.BACKSLASH_ESCAPE,
          {
            className: 'subst',
            begin: '\\$\\{',
            end: '\\}'
          }
        ]
      }
    ]
  };

  const NUMBER = {
    className: 'number',
    variants: [
      { begin: '\\b0[xX][0-9a-fA-F]+\\b' },
      { begin: '\\b0[bB][01]+\\b' },
      { begin: '\\b0[oO][0-7]+\\b' },
      { begin: '\\b[0-9]+\\.[0-9]+([eE][+-]?[0-9]+)?\\b' },
      { begin: '\\b[0-9]+\\b' }
    ]
  };

  const JSX_TAG = {
    className: 'tag',
    begin: '<[A-Z][a-zA-Z0-9]*',
    end: '/?>',
    contains: [
      {
        className: 'name',
        begin: '[A-Z][a-zA-Z0-9]*'
      },
      {
        className: 'attr',
        begin: '[a-zA-Z_][a-zA-Z0-9_-]*(?=\\s*=)'
      },
      {
        begin: '\\{',
        end: '\\}',
        contains: ['self']
      }
    ]
  };

  const JSX_CLOSING_TAG = {
    className: 'tag',
    begin: '</[A-Z][a-zA-Z0-9]*>',
    contains: [
      {
        className: 'name',
        begin: '[A-Z][a-zA-Z0-9]*'
      }
    ]
  };

  const FUNCTION_CALL = {
    className: 'title.function',
    begin: '\\b[a-z_][a-zA-Z0-9_]*(?=\\()'
  };

  const TYPE = {
    className: 'type',
    begin: '\\b[A-Z][a-zA-Z0-9_]*\\b'
  };

  return {
    name: 'L#',
    aliases: ['lsharp', 'ls'],
    keywords: {
      ...KEYWORDS,
      ...MODULE_KEYWORDS,
      ...UI_KEYWORDS,
      ...HOOKS,
      ...LIFECYCLE,
      ...EVENTS,
      ...STYLE_PROPS,
      ...LAYOUT,
      ...FUNCTIONS,
      ...ANIMATION
    },
    contains: [
      COMMENT,
      BLOCK_COMMENT,
      STRING,
      NUMBER,
      JSX_TAG,
      JSX_CLOSING_TAG,
      FUNCTION_CALL,
      TYPE,
      {
        className: 'keyword',
        begin: '\\b(import|export|deport|report)\\b'
      },
      {
        className: 'keyword',
        begin: '\\b(from|direct)\\b'
      },
      {
        className: 'keyword',
        begin: '\\b(component|render)\\b'
      },
      {
        className: 'operator',
        begin: '=>|->|::|\\.\\.\\.'
      }
    ]
  };
}
