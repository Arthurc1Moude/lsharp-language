# L# (L-Sharp) Programming Language

**Native UI Programming Language with Zero Web Dependencies**

L# is a modern programming language designed for building beautiful native desktop applications with React-like syntax but without HTML, CSS, JavaScript, or any web dependencies. Like Flutter, but for native desktop development.

## 🚀 Features

- ✅ **Native Performance** - Compiles to C, runs as native executable
- ✅ **Beautiful UI** - Built-in design system with gradients, animations, and effects
- ✅ **Zero Web Dependencies** - No HTML, CSS, JavaScript, or React
- ✅ **Component-Based** - Familiar React-like component model
- ✅ **Cross-Platform** - Write once, run on Linux, macOS, Windows
- ✅ **OpenGL Support** - Direct access to OpenGL for 3D graphics
- ✅ **C Library Integration** - Import and use any C library directly
- ✅ **Fast Development** - Direct .ls execution without compilation
- ✅ **Memory Management** - Unique `deport` keyword for explicit memory control
- ✅ **Module Inspection** - `report` keyword for debugging modules

## 📦 Installation

```bash
# Clone the repository
git clone https://github.com/Arthurc1Moude/lsharp-language.git
cd lsharp-language

# Install lxc CLI tool
./install-lxc.sh

# Verify installation
lxc --version
```

## 🎯 Quick Start

### Hello World

```lsharp
component HelloWorld {
  render {
    <Container style={{
      background: "linear-gradient(135deg, #667eea 0%, #764ba2 100%)",
      width: "100vw",
      height: "100vh",
      display: "flex",
      alignItems: "center",
      justifyContent: "center"
    }}>
      <Text style={{ fontSize: "48px", color: "#ffffff" }}>
        Hello, L#!
      </Text>
    </Container>
  }
}

export default HelloWorld;
```

Run it:
```bash
lxc hello.ls
```

### Counter App with State

```lsharp
component CounterApp {
  const [count, setCount] = useState(0);
  
  render {
    <Container style={{
      display: "flex",
      flexDirection: "column",
      alignItems: "center",
      gap: "16px"
    }}>
      <Text style={{ fontSize: "48px" }}>
        {count}
      </Text>
      
      <Button onClick={() => setCount(count + 1)}>
        Increment
      </Button>
    </Container>
  }
}

export default CounterApp;
```

## 🔑 Unique Keywords

L# introduces unique keywords not found in other languages:

### Module System

```lsharp
// Import from cloud/big libraries
import ui from { lsharp-ui-package }

// Import from native/local files
import MyComponent, direct './components/MyComponent.ls'

// Import C libraries
import ( printf, malloc ), direct '/usr/include/stdlib.h'

// Unload module from memory (unique to L#!)
deport ui from { lsharp-ui-package }

// Show module information (unique to L#!)
report from { lsharp-ui-package }
```

## 🎨 UI Components

Built-in components for beautiful interfaces:

- **Layout**: Container, Card, Grid, Stack, Box
- **Interactive**: Button, Input, TextArea, Checkbox, Radio, Switch, Slider, Dropdown
- **Display**: Text, Image, Icon, Modal, Tooltip
- **Graphics**: Canvas, GLCanvas (OpenGL)

## 📊 Language Statistics

- **23+ Keywords** - Including unique `deport` and `report`
- **11 Grammar Rules** - Clean, intuitive syntax
- **1.2 MB Code Size** - Compact, efficient compiler
- **200+ Functions** - Complete compiler implementation
- **21 UI Components** - Rich component library
- **30+ Features** - Professional development tools

## 🛠️ CLI Tool (lxc)

```bash
# Run L# script
lxc app.ls

# Create new project
lxc create my-app

# Watch mode (auto-reload)
lxc watch app.ls

# Build to native binary
lxc build app.ls

# List examples
lxc list

# Interactive REPL
lxc repl

# Check installation
lxc doctor
```

## 📚 Documentation

- [Complete Language Guide](LSHARP-COMPLETE-GUIDE.md)
- [Keywords Reference](LSHARP-KEYWORDS.md)
- [Statistics](LSHARP-STATISTICS.md)
- [Native Independence](L#-NATIVE-INDEPENDENCE.md)

## 🎓 Examples

Check out the `examples/` directory for complete working examples:

- Simple counter app
- Chat UI with MoudeStyle design
- Dashboard with analytics
- Advanced animations
- Complete UI showcase
- OpenGL graphics

## 🏗️ Architecture

```
lsharp-language/
├── compiler/          # Full compiler implementation
│   ├── lexer.l       # Lexical analyzer
│   ├── parser.y      # Syntax parser
│   ├── ast.c         # Abstract syntax tree
│   ├── semantic.c    # Semantic analysis
│   ├── codegen.c     # Code generation
│   ├── optimizer.c   # Code optimization
│   └── ui_*.c        # UI system (25+ files)
├── runtime/          # Native runtime
│   ├── lsharp-native-interpreter.c
│   ├── native-window.c
│   └── lsharp-ui-system.c
├── bin/              # CLI tools
│   └── lxc          # L# execution tool
└── syntax/           # Editor support
    ├── vscode-extension/
    ├── sublime-text/
    ├── highlight.js/
    └── prism/
```

## 🔧 Building from Source

### Prerequisites

- GCC compiler
- SDL2 development libraries
- pkg-config
- Make

### Build

```bash
# Build compiler
cd compiler
make

# Build native interpreter
cd ../runtime
make

# Install CLI tool
cd ..
./install-lxc.sh
```

## 🌟 Why L#?

1. **Native Performance** - No JavaScript overhead, runs as fast as C
2. **Beautiful UI** - Built-in design system, no CSS needed
3. **Zero Dependencies** - No node_modules, no web stack
4. **Memory Control** - Explicit memory management with `deport`
5. **Debugging Tools** - Built-in `report` for module inspection
6. **OpenGL Ready** - Direct OpenGL access for 3D graphics
7. **C Integration** - Use any C library seamlessly
8. **Fast Development** - Direct execution, no build step needed
9. **Small Binaries** - 22-29 KB executables
10. **Cross-Platform** - Write once, run everywhere

## 📈 Performance

```
React App (with Node.js):
  - Size: ~200 MB (node_modules)
  - Startup: ~2-3 seconds
  - Memory: ~100-200 MB

L# Native App:
  - Size: 29 KB (binary only)
  - Startup: < 100ms
  - Memory: ~20 MB

Improvement: 7000x smaller, 20x faster!
```

## 🤝 Contributing

Contributions are welcome! Please read our contributing guidelines before submitting PRs.

## 📄 License

MIT License - see LICENSE file for details

## 👥 Author

**Arthur C1 Moude** - [@Arthurc1Moude](https://github.com/Arthurc1Moude)

## 🔗 Links

- [GitHub Repository](https://github.com/Arthurc1Moude/lsharp-language)
- [Documentation](https://github.com/Arthurc1Moude/lsharp-language/wiki)
- [Issue Tracker](https://github.com/Arthurc1Moude/lsharp-language/issues)
- [Discussions](https://github.com/Arthurc1Moude/lsharp-language/discussions)

## ⭐ Star History

If you find L# useful, please consider giving it a star!

---

**L# - Native UI Programming Made Beautiful** 🚀
