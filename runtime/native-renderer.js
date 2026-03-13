/**
 * L# Native Rendering Engine
 * Independent UI system like Flutter - no React dependency
 * Renders directly to Canvas/WebGL/Native APIs
 */

class LSharpNativeRenderer {
  constructor(platform) {
    this.platform = platform;
    this.canvas = null;
    this.ctx = null;
    this.elements = [];
    this.eventListeners = new Map();
    this.animationFrame = null;
    this.theme = 'light';
    
    this.init();
  }

  init() {
    if (this.platform === 'web') {
      this.initWebRenderer();
    } else if (this.platform === 'mobile') {
      this.initMobileRenderer();
    } else if (this.platform === 'desktop') {
      this.initDesktopRenderer();
    }
  }

  initWebRenderer() {
    // Create canvas for native rendering
    this.canvas = document.createElement('canvas');
    this.canvas.width = window.innerWidth;
    this.canvas.height = window.innerHeight;
    this.canvas.style.position = 'fixed';
    this.canvas.style.top = '0';
    this.canvas.style.left = '0';
    this.canvas.style.zIndex = '1000';
    
    document.body.appendChild(this.canvas);
    this.ctx = this.canvas.getContext('2d');
    
    // Handle resize
    window.addEventListener('resize', () => {
      this.canvas.width = window.innerWidth;
      this.canvas.height = window.innerHeight;
      this.render();
    });

    // Handle mouse events
    this.canvas.addEventListener('click', (e) => this.handleClick(e));
    this.canvas.addEventListener('mousemove', (e) => this.handleMouseMove(e));
    
    // Start render loop
    this.startRenderLoop();
  }

  initMobileRenderer() {
    // For mobile, we'll use native canvas or WebGL
    console.log('Initializing L# Mobile Native Renderer');
    this.initWebRenderer(); // For now, use web renderer
  }

  initDesktopRenderer() {
    // For desktop, we'll use Electron's native APIs
    console.log('Initializing L# Desktop Native Renderer');
    this.initWebRenderer(); // For now, use web renderer
  }

  // Core UI Elements
  createElement(type, props = {}, children = []) {
    const element = {
      id: Math.random().toString(36).substr(2, 9),
      type,
      props: {
        x: 0,
        y: 0,
        width: 100,
        height: 30,
        background: '#ffffff',
        color: '#000000',
        fontSize: 16,
        fontFamily: 'system-ui',
        borderRadius: 0,
        padding: 8,
        margin: 0,
        ...props
      },
      children,
      hover: false,
      pressed: false
    };

    this.elements.push(element);
    return element;
  }

  // Layout System (like Flutter's layout)
  createContainer(props, children) {
    return this.createElement('container', {
      display: 'flex',
      flexDirection: 'column',
      alignItems: 'center',
      justifyContent: 'center',
      width: this.canvas?.width || 800,
      height: this.canvas?.height || 600,
      ...props
    }, children);
  }

  createText(text, props) {
    return this.createElement('text', {
      text,
      fontSize: 16,
      color: '#000000',
      fontWeight: 'normal',
      textAlign: 'center',
      ...props
    });
  }

  createButton(text, onClick, props) {
    const button = this.createElement('button', {
      text,
      background: 'linear-gradient(135deg, #667eea 0%, #764ba2 100%)',
      color: '#ffffff',
      borderRadius: 8,
      padding: 12,
      cursor: 'pointer',
      fontSize: 16,
      fontWeight: '500',
      ...props
    });

    if (onClick) {
      this.eventListeners.set(button.id, { click: onClick });
    }

    return button;
  }

  createCard(props, children) {
    return this.createElement('card', {
      background: '#ffffff',
      borderRadius: 12,
      padding: 24,
      boxShadow: '0 4px 12px rgba(0,0,0,0.1)',
      ...props
    }, children);
  }

  // Rendering Engine
  render() {
    if (!this.ctx) return;

    // Clear canvas
    this.ctx.clearRect(0, 0, this.canvas.width, this.canvas.height);

    // Set background
    this.ctx.fillStyle = this.theme === 'light' ? '#f5f7fa' : '#1a1a1a';
    this.ctx.fillRect(0, 0, this.canvas.width, this.canvas.height);

    // Render all elements
    this.elements.forEach(element => {
      this.renderElement(element);
    });
  }

  renderElement(element) {
    const { type, props } = element;

    // Save context
    this.ctx.save();

    // Apply transforms
    this.ctx.translate(props.x, props.y);

    // Apply opacity if specified
    if (props.opacity) {
      this.ctx.globalAlpha = props.opacity;
    }

    switch (type) {
      case 'container':
        this.renderContainer(element);
        break;
      case 'text':
        this.renderText(element);
        break;
      case 'button':
        this.renderButton(element);
        break;
      case 'card':
        this.renderCard(element);
        break;
      case 'header':
        this.renderHeader(element);
        break;
      case 'messages':
        this.renderMessagesArea(element);
        break;
      case 'message-bubble':
        this.renderMessageBubble(element);
        break;
      case 'input-section':
        this.renderInputSection(element);
        break;
      case 'input-wrapper':
        this.renderInputWrapper(element);
        break;
      case 'textarea':
        this.renderTextarea(element);
        break;
      case 'bottom-actions':
        this.renderBottomActions(element);
        break;
      case 'model-menu':
        this.renderModelMenu(element);
        break;
      case 'particle':
        this.renderParticle(element);
        break;
    }

    // Render children
    if (element.children) {
      element.children.forEach(child => {
        if (typeof child === 'object') {
          this.renderElement(child);
        }
      });
    }

    // Restore context
    this.ctx.restore();
  }

  renderContainer(element) {
    const { props } = element;
    
    // Background
    if (props.background) {
      this.ctx.fillStyle = props.background;
      this.ctx.fillRect(0, 0, props.width, props.height);
    }

    // Layout children (simplified flexbox)
    if (props.display === 'flex') {
      this.layoutFlexChildren(element);
    }
  }

  renderText(element) {
    const { props } = element;
    
    this.ctx.fillStyle = props.color;
    this.ctx.font = `${props.fontWeight} ${props.fontSize}px ${props.fontFamily}`;
    this.ctx.textAlign = props.textAlign || 'left';
    this.ctx.textBaseline = 'middle';
    
    this.ctx.fillText(props.text, 0, props.fontSize / 2);
  }

  renderButton(element) {
    const { props } = element;
    
    // Button background
    if (props.background.includes('gradient')) {
      // Create gradient
      const gradient = this.ctx.createLinearGradient(0, 0, props.width, 0);
      gradient.addColorStop(0, '#667eea');
      gradient.addColorStop(1, '#764ba2');
      this.ctx.fillStyle = gradient;
    } else {
      this.ctx.fillStyle = props.background;
    }

    // Draw rounded rectangle
    this.drawRoundedRect(0, 0, props.width, props.height, props.borderRadius);
    this.ctx.fill();

    // Button text
    this.ctx.fillStyle = props.color;
    this.ctx.font = `${props.fontWeight} ${props.fontSize}px ${props.fontFamily}`;
    this.ctx.textAlign = 'center';
    this.ctx.textBaseline = 'middle';
    
    this.ctx.fillText(props.text, props.width / 2, props.height / 2);

    // Hover effect
    if (element.hover) {
      this.ctx.fillStyle = 'rgba(255, 255, 255, 0.1)';
      this.drawRoundedRect(0, 0, props.width, props.height, props.borderRadius);
      this.ctx.fill();
    }
  }

  renderCard(element) {
    const { props } = element;
    
    // Card shadow (simplified)
    this.ctx.fillStyle = 'rgba(0, 0, 0, 0.1)';
    this.drawRoundedRect(2, 2, props.width, props.height, props.borderRadius);
    this.ctx.fill();

    // Card background
    this.ctx.fillStyle = props.background;
    this.drawRoundedRect(0, 0, props.width, props.height, props.borderRadius);
    this.ctx.fill();
  }

  drawRoundedRect(x, y, width, height, radius) {
    this.ctx.beginPath();
    this.ctx.moveTo(x + radius, y);
    this.ctx.lineTo(x + width - radius, y);
    this.ctx.quadraticCurveTo(x + width, y, x + width, y + radius);
    this.ctx.lineTo(x + width, y + height - radius);
    this.ctx.quadraticCurveTo(x + width, y + height, x + width - radius, y + height);
    this.ctx.lineTo(x + radius, y + height);
    this.ctx.quadraticCurveTo(x, y + height, x, y + height - radius);
    this.ctx.lineTo(x, y + radius);
    this.ctx.quadraticCurveTo(x, y, x + radius, y);
    this.ctx.closePath();
  }

  layoutFlexChildren(container) {
    const { props, children } = container;
    let currentY = props.padding || 0;
    let currentX = props.padding || 0;

    children.forEach((child, index) => {
      if (typeof child === 'object') {
        if (props.flexDirection === 'column') {
          child.props.x = currentX;
          child.props.y = currentY;
          currentY += child.props.height + (child.props.margin || 10);
        } else {
          child.props.x = currentX;
          child.props.y = currentY;
          currentX += child.props.width + (child.props.margin || 10);
        }

        // Center alignment
        if (props.alignItems === 'center') {
          if (props.flexDirection === 'column') {
            child.props.x = (props.width - child.props.width) / 2;
          } else {
            child.props.y = (props.height - child.props.height) / 2;
          }
        }
      }
    });
  }

  // Event Handling
  handleClick(e) {
    const rect = this.canvas.getBoundingClientRect();
    const x = e.clientX - rect.left;
    const y = e.clientY - rect.top;

    this.elements.forEach(element => {
      if (this.isPointInElement(x, y, element)) {
        const listener = this.eventListeners.get(element.id);
        if (listener && listener.click) {
          listener.click();
        }
      }
    });
  }

  handleMouseMove(e) {
    const rect = this.canvas.getBoundingClientRect();
    const x = e.clientX - rect.left;
    const y = e.clientY - rect.top;

    this.elements.forEach(element => {
      const wasHover = element.hover;
      element.hover = this.isPointInElement(x, y, element);
      
      if (element.hover !== wasHover) {
        this.render(); // Re-render on hover change
      }
    });

    // Update cursor
    const hoverElement = this.elements.find(el => el.hover && el.type === 'button');
    this.canvas.style.cursor = hoverElement ? 'pointer' : 'default';
  }

  isPointInElement(x, y, element) {
    const { props } = element;
    return x >= props.x && x <= props.x + props.width &&
           y >= props.y && y <= props.y + props.height;
  }

  // Animation System
  startRenderLoop() {
    const loop = () => {
      this.render();
      this.animationFrame = requestAnimationFrame(loop);
    };
    loop();
  }

  stopRenderLoop() {
    if (this.animationFrame) {
      cancelAnimationFrame(this.animationFrame);
    }
  }

  // Theme System
  setTheme(theme) {
    this.theme = theme;
    this.render();
  }

  // State Management
  useState(initialValue) {
    let value = initialValue;
    const setValue = (newValue) => {
      value = typeof newValue === 'function' ? newValue(value) : newValue;
      this.render(); // Re-render on state change
    };
    return [() => value, setValue];
  }

  // Cleanup
  destroy() {
    this.stopRenderLoop();
    if (this.canvas && this.canvas.parentNode) {
      this.canvas.parentNode.removeChild(this.canvas);
    }
    this.elements = [];
    this.eventListeners.clear();
  }
}

// L# Widget System (like Flutter widgets)
class LSharpWidget {
  constructor(renderer) {
    this.renderer = renderer;
  }

  build() {
    // Override in subclasses
    return null;
  }
}

class LSharpApp extends LSharpWidget {
  constructor(renderer) {
    super(renderer);
    this.state = {
      count: 0,
      theme: 'light'
    };
  }

  build() {
    const container = this.renderer.createContainer({
      background: this.state.theme === 'light' ? '#f5f7fa' : '#1a1a1a'
    });

    // Title
    const title = this.renderer.createText('Welcome to L# Framework!', {
      fontSize: 32,
      fontWeight: 'bold',
      color: this.state.theme === 'light' ? '#333' : '#fff',
      y: 100,
      width: 600,
      height: 40
    });

    // Subtitle
    const subtitle = this.renderer.createText('Independent UI Framework - No React Needed!', {
      fontSize: 18,
      color: this.state.theme === 'light' ? '#666' : '#ccc',
      y: 160,
      width: 500,
      height: 25
    });

    // Card
    const card = this.renderer.createCard({
      x: (this.renderer.canvas.width - 300) / 2,
      y: 220,
      width: 300,
      height: 200,
      background: this.state.theme === 'light' ? '#fff' : '#2d2d2d'
    });

    // Counter text
    const counterText = this.renderer.createText(`Counter: ${this.state.count}`, {
      fontSize: 24,
      fontWeight: 'bold',
      color: this.state.theme === 'light' ? '#333' : '#fff',
      x: 150,
      y: 50,
      width: 200,
      height: 30,
      textAlign: 'center'
    });

    // Increment button
    const incrementBtn = this.renderer.createButton('Increment', () => {
      this.state.count++;
      this.rebuild();
    }, {
      x: 50,
      y: 100,
      width: 100,
      height: 40
    });

    // Theme button
    const themeBtn = this.renderer.createButton('Toggle Theme', () => {
      this.state.theme = this.state.theme === 'light' ? 'dark' : 'light';
      this.rebuild();
    }, {
      x: 160,
      y: 100,
      width: 120,
      height: 40,
      background: this.state.theme === 'light' ? '#333' : '#fff',
      color: this.state.theme === 'light' ? '#fff' : '#333'
    });

    // Platform badges
    const platformText = this.renderer.createText('🌐 Web • 📱 Mobile • 🖥️ Desktop • 🚀 Server', {
      fontSize: 14,
      color: this.state.theme === 'light' ? '#666' : '#ccc',
      y: 480,
      width: 400,
      height: 20
    });

    // Add children to card
    card.children = [counterText, incrementBtn, themeBtn];
    
    // Add all to container
    container.children = [title, subtitle, card, platformText];

    return container;
  }

  rebuild() {
    this.renderer.elements = [];
    this.build();
    this.renderer.render();
  }
}

module.exports = { LSharpNativeRenderer, LSharpWidget, LSharpApp };

  // New rendering methods for MoudeStyle design
  renderHeader(element) {
    const { props } = element;
    
    // Header background with gradient
    if (props.background.includes('gradient')) {
      const gradient = this.ctx.createLinearGradient(0, 0, props.width, 0);
      gradient.addColorStop(0, '#667eea');
      gradient.addColorStop(1, '#764ba2');
      this.ctx.fillStyle = gradient;
    } else {
      this.ctx.fillStyle = props.background;
    }

    this.drawRoundedRect(0, 0, props.width, props.height, props.borderRadius);
    this.ctx.fill();
  }

  renderMessagesArea(element) {
    const { props } = element;
    
    // Messages area background
    this.ctx.fillStyle = props.background;
    this.drawRoundedRect(0, 0, props.width, props.height, props.borderRadius);
    this.ctx.fill();

    // Subtle inner shadow effect
    this.ctx.strokeStyle = 'rgba(0,0,0,0.05)';
    this.ctx.lineWidth = 1;
    this.drawRoundedRect(1, 1, props.width - 2, props.height - 2, props.borderRadius - 1);
    this.ctx.stroke();
  }

  renderMessageBubble(element) {
    const { props } = element;
    
    // Drop shadow
    this.ctx.fillStyle = 'rgba(0,0,0,0.1)';
    this.drawRoundedRect(2, 2, props.width, props.height, props.borderRadius);
    this.ctx.fill();

    // Bubble background
    if (props.background.includes('gradient')) {
      const gradient = this.ctx.createLinearGradient(0, 0, props.width, 0);
      gradient.addColorStop(0, '#667eea');
      gradient.addColorStop(1, '#764ba2');
      this.ctx.fillStyle = gradient;
    } else {
      this.ctx.fillStyle = props.background;
    }

    this.drawRoundedRect(0, 0, props.width, props.height, props.borderRadius);
    this.ctx.fill();

    // Subtle highlight
    this.ctx.fillStyle = 'rgba(255,255,255,0.1)';
    this.drawRoundedRect(0, 0, props.width, 2, props.borderRadius);
    this.ctx.fill();
  }

  renderInputSection(element) {
    const { props } = element;
    
    // Input section background
    this.ctx.fillStyle = props.background;
    this.drawRoundedRect(0, 0, props.width, props.height, props.borderRadius);
    this.ctx.fill();

    // Top border shadow
    this.ctx.fillStyle = 'rgba(0,0,0,0.05)';
    this.ctx.fillRect(0, 0, props.width, 1);
  }

  renderInputWrapper(element) {
    const { props } = element;
    
    // Input wrapper background
    this.ctx.fillStyle = props.background;
    this.drawRoundedRect(0, 0, props.width, props.height, props.borderRadius);
    this.ctx.fill();

    // Border
    if (props.border) {
      this.ctx.strokeStyle = '#e2e8f0';
      this.ctx.lineWidth = 2;
      this.drawRoundedRect(1, 1, props.width - 2, props.height - 2, props.borderRadius - 1);
      this.ctx.stroke();
    }
  }

  renderTextarea(element) {
    const { props } = element;
    
    // Placeholder text
    if (props.placeholder && !props.value) {
      this.ctx.fillStyle = '#9ca3af';
      this.ctx.font = `${props.fontSize}px ${props.fontFamily || 'system-ui'}`;
      this.ctx.textAlign = 'left';
      this.ctx.textBaseline = 'top';
      this.ctx.fillText(props.placeholder, 0, 0);
    }

    // Cursor (if focused)
    if (props.focused) {
      this.ctx.fillStyle = '#667eea';
      this.ctx.fillRect(0, 0, 2, props.fontSize);
    }
  }

  renderBottomActions(element) {
    const { props } = element;
    // Transparent background - no rendering needed
  }

  renderModelMenu(element) {
    const { props } = element;
    
    // Menu shadow
    this.ctx.fillStyle = 'rgba(0,0,0,0.15)';
    this.drawRoundedRect(2, 2, props.width, props.height, props.borderRadius);
    this.ctx.fill();

    // Menu background
    this.ctx.fillStyle = props.background;
    this.drawRoundedRect(0, 0, props.width, props.height, props.borderRadius);
    this.ctx.fill();

    // Menu border
    this.ctx.strokeStyle = '#e5e7eb';
    this.ctx.lineWidth = 1;
    this.drawRoundedRect(0.5, 0.5, props.width - 1, props.height - 1, props.borderRadius);
    this.ctx.stroke();
  }

  renderParticle(element) {
    const { props } = element;
    
    // Animated particle
    this.ctx.fillStyle = props.background;
    this.ctx.beginPath();
    this.ctx.arc(props.width/2, props.height/2, props.width/2, 0, Math.PI * 2);
    this.ctx.fill();

    // Animate particles
    props.x += Math.sin(Date.now() * 0.001 + props.y * 0.01) * 0.5;
    props.y += 0.2;
    
    if (props.y > this.canvas.height) {
      props.y = -10;
      props.x = Math.random() * this.canvas.width;
    }
  }

  // Enhanced button rendering with better gradients
  renderButton(element) {
    const { props } = element;
    
    // Button shadow
    if (props.boxShadow) {
      this.ctx.fillStyle = 'rgba(102,126,234,0.3)';
      this.drawRoundedRect(0, 4, props.width, props.height, props.borderRadius);
      this.ctx.fill();
    }

    // Button background
    if (props.background.includes('gradient')) {
      const gradient = this.ctx.createLinearGradient(0, 0, props.width, 0);
      gradient.addColorStop(0, '#667eea');
      gradient.addColorStop(1, '#764ba2');
      this.ctx.fillStyle = gradient;
    } else {
      this.ctx.fillStyle = props.background;
    }

    this.drawRoundedRect(0, 0, props.width, props.height, props.borderRadius);
    this.ctx.fill();

    // Button border
    if (props.border) {
      this.ctx.strokeStyle = props.border.split(' ')[2]; // Extract color
      this.ctx.lineWidth = 1;
      this.drawRoundedRect(0.5, 0.5, props.width - 1, props.height - 1, props.borderRadius);
      this.ctx.stroke();
    }

    // Button text
    this.ctx.fillStyle = props.color;
    this.ctx.font = `${props.fontWeight || 'normal'} ${props.fontSize}px ${props.fontFamily || 'system-ui'}`;
    this.ctx.textAlign = props.textAlign || 'center';
    this.ctx.textBaseline = 'middle';
    
    this.ctx.fillText(props.text, props.width / 2, props.height / 2);

    // Hover effect
    if (element.hover) {
      this.ctx.fillStyle = 'rgba(255, 255, 255, 0.1)';
      this.drawRoundedRect(0, 0, props.width, props.height, props.borderRadius);
      this.ctx.fill();
    }
  }

  // Enhanced container rendering with gradients
  renderContainer(element) {
    const { props } = element;
    
    // Background with gradient support
    if (props.background) {
      if (props.background.includes('gradient')) {
        const gradient = this.ctx.createLinearGradient(0, 0, props.width, props.height);
        gradient.addColorStop(0, '#667eea');
        gradient.addColorStop(1, '#764ba2');
        this.ctx.fillStyle = gradient;
      } else {
        this.ctx.fillStyle = props.background;
      }
      this.ctx.fillRect(0, 0, props.width, props.height);
    }

    // Layout children (simplified flexbox)
    if (props.display === 'flex') {
      this.layoutFlexChildren(element);
    }
  }