/**
 * Prism.js syntax highlighting for L# (L-Sharp)
 * @author L# Team
 * @license MIT
 */

(function (Prism) {
  Prism.languages.lsharp = {
    'comment': [
      {
        pattern: /(^|[^\\])\/\*[\s\S]*?(?:\*\/|$)/,
        lookbehind: true,
        greedy: true
      },
      {
        pattern: /(^|[^\\:])\/\/.*/,
        lookbehind: true,
        greedy: true
      }
    ],
    'string': [
      {
        pattern: /(["'`])(?:\\[\s\S]|(?!\1)[^\\])*\1/,
        greedy: true
      },
      {
        pattern: /`(?:\\[\s\S]|[^\\`]|\$\{[^}]*\})*`/,
        greedy: true,
        inside: {
          'interpolation': {
            pattern: /\$\{[^}]+\}/,
            inside: {
              'punctuation': /^\$\{|\}$/,
              rest: Prism.languages.lsharp
            }
          }
        }
      }
    ],
    'module-keyword': {
      pattern: /\b(?:import|export|deport|report|from|direct|use|as)\b/,
      alias: 'keyword'
    },
    'ui-keyword': {
      pattern: /\b(?:component|render|props|state|computed)\b/,
      alias: 'keyword'
    },
    'hook': {
      pattern: /\b(?:useState|useEffect|useContext|useRef|useMemo|useCallback|useReducer|useLayoutEffect|useImperativeHandle|useDebugValue)\b/,
      alias: 'function'
    },
    'lifecycle': {
      pattern: /\b(?:onMount|onUnmount|onUpdate|beforeRender|afterRender|shouldUpdate|onError)\b/,
      alias: 'function'
    },
    'event': {
      pattern: /\b(?:onClick|onChange|onSubmit|onInput|onFocus|onBlur|onKeyDown|onKeyUp|onKeyPress|onMouseDown|onMouseUp|onMouseMove|onMouseEnter|onMouseLeave|onMouseOver|onMouseOut|onWheel|onScroll|onDrag|onDragStart|onDragEnd|onDragEnter|onDragLeave|onDragOver|onDrop|onTouchStart|onTouchMove|onTouchEnd|onTouchCancel)\b/,
      alias: 'function'
    },
    'ui-component': {
      pattern: /\b(?:Container|Card|Button|Input|Text|Image|Icon|Canvas|GLCanvas|Grid|Stack|Box|Modal|Tooltip|TextArea|Checkbox|Radio|Switch|Slider|Dropdown)\b/,
      alias: 'class-name'
    },
    'style-property': {
      pattern: /\b(?:style|className|width|height|margin|padding|border|borderRadius|background|backgroundColor|color|fontSize|fontWeight|fontFamily|display|position|top|right|bottom|left|zIndex|opacity|transform|transition|animation|boxShadow|textShadow|overflow|cursor)\b/,
      alias: 'property'
    },
    'layout-keyword': {
      pattern: /\b(?:layout|flex|grid|stack|absolute|relative|fixed|sticky)\b/,
      alias: 'keyword'
    },
    'layout-property': {
      pattern: /\b(?:flexDirection|flexWrap|flexGrow|flexShrink|flexBasis|alignItems|alignContent|alignSelf|justifyContent|justifyItems|justifySelf|gridTemplateColumns|gridTemplateRows|gridTemplateAreas|gridColumn|gridRow|gridArea|gridGap|gridColumnGap|gridRowGap|gridAutoFlow|gridAutoColumns|gridAutoRows)\b/,
      alias: 'property'
    },
    'layout-value': {
      pattern: /\b(?:row|column|wrap|nowrap|center|start|end|stretch|baseline|space-between|space-around|space-evenly)\b/,
      alias: 'constant'
    },
    'gradient-function': {
      pattern: /\b(?:linearGradient|radialGradient|conicGradient)\b/,
      alias: 'function'
    },
    'transform-function': {
      pattern: /\b(?:translate|translateX|translateY|scale|scaleX|scaleY|rotate|rotateX|rotateY|skew|skewX|skewY|matrix|perspective)\b/,
      alias: 'function'
    },
    'filter-function': {
      pattern: /\b(?:blur|brightness|contrast|grayscale|hueRotate|invert|saturate|sepia|dropShadow)\b/,
      alias: 'function'
    },
    'animation-keyword': {
      pattern: /\b(?:animation|keyframes|transition)\b/,
      alias: 'keyword'
    },
    'animation-value': {
      pattern: /\b(?:ease|ease-in|ease-out|ease-in-out|linear|step-start|step-end|infinite|normal|reverse|alternate|alternate-reverse|forwards|backwards|both|running|paused)\b/,
      alias: 'constant'
    },
    'keyword': /\b(?:if|else|elif|match|case|for|while|loop|break|continue|return|yield|await|async|let|const|var|fn|func|function|class|struct|enum|interface|trait|impl|type|alias|pub|priv|protected|static|final|abstract|virtual|override|mut|ref|unsafe|extern|dyn|macro|derive|new|delete|sizeof|typeof|instanceof|as|in|is)\b/,
    'boolean': /\b(?:true|false)\b/,
    'null': /\b(?:null|nil|undefined)\b/,
    'this': /\b(?:this|self|super|base)\b/,
    'number': /\b(?:0[xX][0-9a-fA-F]+|0[bB][01]+|0[oO][0-7]+|[0-9]+\.?[0-9]*(?:[eE][+-]?[0-9]+)?)\b/,
    'function': /\b[a-z_][a-zA-Z0-9_]*(?=\s*\()/,
    'builtin': /\b(?:printf|scanf|malloc|free|memcpy|strlen|strcmp|glBegin|glEnd|glVertex3f|glColor3f)\b/,
    'class-name': /\b[A-Z][a-zA-Z0-9_]*\b/,
    'operator': /=>|->|::|\.\.\.|\+\+|--|&&|\|\||[+\-*\/%<>=!&|^~?]=?/,
    'punctuation': /[{}[\]();,.:]/
  };

  // JSX support
  Prism.languages.lsharp['jsx-tag'] = {
    pattern: /<\/?[A-Z][a-zA-Z0-9]*(?:\s+[a-zA-Z_][a-zA-Z0-9_-]*(?:=(?:"[^"]*"|'[^']*'|\{[^}]*\}))?)*\s*\/?>/,
    inside: {
      'tag': {
        pattern: /^<\/?[A-Z][a-zA-Z0-9]*/,
        inside: {
          'punctuation': /^<\/?/,
          'class-name': /[A-Z][a-zA-Z0-9]*/
        }
      },
      'attr-name': /[a-zA-Z_][a-zA-Z0-9_-]*(?==)/,
      'attr-value': {
        pattern: /=(?:"[^"]*"|'[^']*'|\{[^}]*\})/,
        inside: {
          'punctuation': [
            /^=/,
            {
              pattern: /^(\{|\})$/,
              lookbehind: true
            }
          ],
          'expression': {
            pattern: /\{[^}]*\}/,
            inside: Prism.languages.lsharp
          }
        }
      },
      'punctuation': /\/?>$/
    }
  };

  Prism.languages.ls = Prism.languages.lsharp;

}(Prism));
