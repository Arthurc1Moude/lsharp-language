#!/usr/bin/env node
/**
 * L# Language Interpreter
 * Executes .ls files directly without compilation
 */

const fs = require('fs');
const path = require('path');
const vm = require('vm');

class LSharpInterpreter {
  constructor() {
    this.context = {
      console,
      require,
      process,
      __dirname,
      __filename,
      // L# built-in modules
      ui: this.createUIModule(),
      state: this.createStateModule(),
      effects: this.createEffectsModule(),
      animations: this.createAnimationsModule(),
      charts: this.createChartsModule()
    };
  }

  createUIModule() {
    return {
      createElement: (type, props, ...children) => {
        return { type, props: props || {}, children };
      },
      render: (element, container) => {
        console.log('Rendering:', JSON.stringify(element, null, 2));
      }
    };
  }

  createStateModule() {
    return {
      useState: (initialValue) => {
        let value = initialValue;
        const setValue = (newValue) => {
          value = typeof newValue === 'function' ? newValue(value) : newValue;
        };
        return [value, setValue];
      },
      useEffect: (effect, deps) => {
        effect();
      },
      useRef: (initialValue) => {
        return { current: initialValue };
      }
    };
  }

  createEffectsModule() {
    return {
      fadeIn: (duration) => `fadeIn ${duration}ms`,
      fadeOut: (duration) => `fadeOut ${duration}ms`,
      slideIn: (duration) => `slideIn ${duration}ms`
    };
  }

  createAnimationsModule() {
    return {
      fadeIn: (duration) => ({ animation: `fadeIn ${duration}ms` }),
      fadeInUp: (duration, delay) => ({ animation: `fadeInUp ${duration}ms ${delay}ms` }),
      slideInLeft: (duration) => ({ animation: `slideInLeft ${duration}ms` }),
      bounce: (duration, delay) => ({ animation: `bounce ${duration}ms ${delay}ms` }),
      scale: (duration) => ({ animation: `scale ${duration}ms` })
    };
  }

  createChartsModule() {
    return {
      LineChart: (props) => ({ type: 'LineChart', ...props }),
      BarChart: (props) => ({ type: 'BarChart', ...props }),
      PieChart: (props) => ({ type: 'PieChart', ...props }),
      DoughnutChart: (props) => ({ type: 'DoughnutChart', ...props })
    };
  }

  parseLSharp(code) {
    // Transform L# syntax to JavaScript
    let jsCode = code;

    // Transform imports - use this.context instead of context
    jsCode = jsCode.replace(/import\s+(\w+)\s+from\s+"lsharp\/(\w+)";/g, 
      'const $1 = $2Module;');

    // Transform state declarations
    jsCode = jsCode.replace(/state\s+(\w+)\s*=\s*({[^}]+});/g, 
      'let $1 = $2;');

    // Transform component declarations
    jsCode = jsCode.replace(/component\s+(\w+)\s*{/g, 
      'function $1(props) {');

    // Transform component with props
    jsCode = jsCode.replace(/component\s+(\w+)\s*\(\s*{([^}]+)}\s*\)\s*{/g, 
      'function $1({ $2 }) {');

    // Transform render blocks
    jsCode = jsCode.replace(/render\s*{/g, 'return (');
    
    // Close render blocks
    jsCode = jsCode.replace(/}\s*}\s*$/gm, '});\n}');

    // Transform JSX-like syntax to createElement calls
    // This is simplified - a real parser would be more robust
    
    return jsCode;
  }

  execute(filePath) {
    try {
      const code = fs.readFileSync(filePath, 'utf-8');
      console.log(`\n${'='.repeat(60)}`);
      console.log(`Executing L# file: ${filePath}`);
      console.log('='.repeat(60));

      const jsCode = this.parseLSharp(code);
      
      console.log('\n--- Transpiled JavaScript ---');
      console.log(jsCode.substring(0, 500) + '...\n');

      // Create context with all modules available
      const contextObj = {
        ...this.context,
        uiModule: this.context.ui,
        stateModule: this.context.state,
        effectsModule: this.context.effects,
        animationsModule: this.context.animations,
        chartsModule: this.context.charts
      };

      const script = new vm.Script(jsCode, {
        filename: filePath
      });

      const context = vm.createContext(contextObj);
      const result = script.runInContext(context);

      console.log('\n--- Execution Result ---');
      console.log('File executed successfully!');
      
      if (result) {
        console.log('Return value:', result);
      }

      return result;
    } catch (error) {
      console.error('\n--- Execution Error ---');
      console.error('Error:', error.message);
      console.error('Stack:', error.stack);
      throw error;
    }
  }

  runFile(filePath) {
    const absolutePath = path.resolve(filePath);
    
    if (!fs.existsSync(absolutePath)) {
      console.error(`Error: File not found: ${absolutePath}`);
      process.exit(1);
    }

    if (!absolutePath.endsWith('.ls')) {
      console.error('Error: File must have .ls extension');
      process.exit(1);
    }

    try {
      this.execute(absolutePath);
      console.log('\n✓ Execution completed successfully\n');
    } catch (error) {
      console.error('\n✗ Execution failed\n');
      process.exit(1);
    }
  }
}

// CLI Interface
if (require.main === module) {
  const args = process.argv.slice(2);

  if (args.length === 0) {
    console.log('L# Interpreter');
    console.log('Usage: node lsharp-interpreter.js <file.ls>');
    console.log('');
    console.log('Example:');
    console.log('  node lsharp-interpreter.js ../examples/complete_chat_app.ls');
    process.exit(1);
  }

  const interpreter = new LSharpInterpreter();
  interpreter.runFile(args[0]);
}

module.exports = LSharpInterpreter;
