/* Performance Optimization System for L# Language
 * Code splitting, lazy loading, memoization, virtualization, caching
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "ui_renderer.h"

/* Code Splitting */
typedef struct CodeChunk {
    char *name;
    char *path;
    char **dependencies;
    int dep_count;
    int loaded;
    struct CodeChunk *next;
} CodeChunk;

typedef struct {
    CodeChunk *chunks;
    int chunk_count;
} CodeSplitter;

CodeSplitter* create_code_splitter() {
    CodeSplitter *cs = malloc(sizeof(CodeSplitter));
    cs->chunks = NULL;
    cs->chunk_count = 0;
    return cs;
}

CodeChunk* create_code_chunk(const char *name, const char *path) {
    CodeChunk *chunk = malloc(sizeof(CodeChunk));
    chunk->name = strdup(name);
    chunk->path = strdup(path);
    chunk->dependencies = NULL;
    chunk->dep_count = 0;
    chunk->loaded = 0;
    chunk->next = NULL;
    return chunk;
}

void add_chunk_dependency(CodeChunk *chunk, const char *dep) {
    chunk->dep_count++;
    chunk->dependencies = realloc(chunk->dependencies, chunk->dep_count * sizeof(char*));
    chunk->dependencies[chunk->dep_count - 1] = strdup(dep);
}

char* generate_dynamic_import(const char *chunk_name, const char *path) {
    char *code = malloc(1024);
    snprintf(code, 1024,
        "const load%s = () => {\n"
        "  return import(/* webpackChunkName: \"%s\" */ '%s')\n"
        "    .then(module => {\n"
        "      console.log('%s loaded');\n"
        "      return module;\n"
        "    })\n"
        "    .catch(error => {\n"
        "      console.error('Failed to load %s:', error);\n"
        "    });\n"
        "};\n",
        chunk_name, chunk_name, path, chunk_name, chunk_name);
    return code;
}

/* Lazy Loading Components */
char* generate_lazy_component(const char *component_name, const char *import_path) {
    char *code = malloc(1024);
    snprintf(code, 1024,
        "const %s = lazy(() => import('%s'));\n"
        "\n"
        "const %sWithSuspense = (props) => (\n"
        "  <Suspense fallback={<div>Loading %s...</div>}>\n"
        "    <%s {...props} />\n"
        "  </Suspense>\n"
        ");\n",
        component_name, import_path,
        component_name, component_name, component_name);
    return code;
}

/* Memoization System */
typedef struct MemoEntry {
    char *key;
    void *value;
    time_t timestamp;
    int hit_count;
    struct MemoEntry *next;
} MemoEntry;

typedef struct {
    MemoEntry *entries;
    int size;
    int max_size;
    int ttl;
} MemoCache;

MemoCache* create_memo_cache(int max_size, int ttl) {
    MemoCache *cache = malloc(sizeof(MemoCache));
    cache->entries = NULL;
    cache->size = 0;
    cache->max_size = max_size;
    cache->ttl = ttl;
    return cache;
}

void* memo_get(MemoCache *cache, const char *key) {
    MemoEntry *current = cache->entries;
    time_t now = time(NULL);
    
    while (current) {
        if (strcmp(current->key, key) == 0) {
            if (cache->ttl > 0 && (now - current->timestamp) > cache->ttl) {
                return NULL;
            }
            current->hit_count++;
            return current->value;
        }
        current = current->next;
    }
    return NULL;
}

void memo_set(MemoCache *cache, const char *key, void *value) {
    MemoEntry *entry = malloc(sizeof(MemoEntry));
    entry->key = strdup(key);
    entry->value = value;
    entry->timestamp = time(NULL);
    entry->hit_count = 0;
    entry->next = cache->entries;
    cache->entries = entry;
    cache->size++;
    
    if (cache->size > cache->max_size) {
        MemoEntry *prev = NULL;
        MemoEntry *current = cache->entries;
        MemoEntry *lru = cache->entries;
        int min_hits = cache->entries->hit_count;
        
        while (current) {
            if (current->hit_count < min_hits) {
                min_hits = current->hit_count;
                lru = current;
            }
            current = current->next;
        }
        
        current = cache->entries;
        while (current) {
            if (current == lru) {
                if (prev) {
                    prev->next = current->next;
                } else {
                    cache->entries = current->next;
                }
                free(current->key);
                free(current);
                cache->size--;
                break;
            }
            prev = current;
            current = current->next;
        }
    }
}

char* generate_memo_hook(const char *func_name, char **deps, int dep_count) {
    char *code = malloc(2048);
    snprintf(code, 256, "const memoized%s = useMemo(() => {\n  return ", func_name);
    strcat(code, func_name);
    strcat(code, "();\n}, [");
    
    for (int i = 0; i < dep_count; i++) {
        strcat(code, deps[i]);
        if (i < dep_count - 1) strcat(code, ", ");
    }
    strcat(code, "]);\n");
    
    return code;
}

/* Virtual Scrolling */
typedef struct {
    int total_items;
    int visible_items;
    int item_height;
    int container_height;
    int scroll_top;
    int overscan;
    int start_index;
    int end_index;
    int offset_y;
} VirtualScrollConfig;

VirtualScrollConfig* create_virtual_scroll_config(int total_items, int item_height, int container_height) {
    VirtualScrollConfig *config = malloc(sizeof(VirtualScrollConfig));
    config->total_items = total_items;
    config->item_height = item_height;
    config->container_height = container_height;
    config->scroll_top = 0;
    config->overscan = 3;
    config->visible_items = (container_height / item_height) + 1;
    config->start_index = 0;
    config->end_index = config->visible_items + config->overscan;
    config->offset_y = 0;
    return config;
}

void update_virtual_scroll(VirtualScrollConfig *config, int scroll_top) {
    config->scroll_top = scroll_top;
    config->start_index = (scroll_top / config->item_height) - config->overscan;
    if (config->start_index < 0) config->start_index = 0;
    
    config->end_index = config->start_index + config->visible_items + (config->overscan * 2);
    if (config->end_index > config->total_items) {
        config->end_index = config->total_items;
    }
    
    config->offset_y = config->start_index * config->item_height;
}

char* generate_virtual_scroll_component(VirtualScrollConfig *config) {
    char *code = malloc(4096);
    snprintf(code, 4096,
        "const VirtualScroll = ({ items, itemHeight, containerHeight }) => {\n"
        "  const [scrollTop, setScrollTop] = useState(0);\n"
        "  const totalHeight = items.length * itemHeight;\n"
        "  const visibleItems = Math.ceil(containerHeight / itemHeight) + 1;\n"
        "  const overscan = 3;\n"
        "  \n"
        "  const startIndex = Math.max(0, Math.floor(scrollTop / itemHeight) - overscan);\n"
        "  const endIndex = Math.min(items.length, startIndex + visibleItems + overscan * 2);\n"
        "  const offsetY = startIndex * itemHeight;\n"
        "  \n"
        "  const visibleItems = items.slice(startIndex, endIndex);\n"
        "  \n"
        "  return (\n"
        "    <div\n"
        "      style={{ height: containerHeight, overflow: 'auto' }}\n"
        "      onScroll={(e) => setScrollTop(e.target.scrollTop)}\n"
        "    >\n"
        "      <div style={{ height: totalHeight, position: 'relative' }}>\n"
        "        <div style={{ transform: `translateY(${offsetY}px)` }}>\n"
        "          {visibleItems.map((item, index) => (\n"
        "            <div key={startIndex + index} style={{ height: itemHeight }}>\n"
        "              {item}\n"
        "            </div>\n"
        "          ))}\n"
        "        </div>\n"
        "      </div>\n"
        "    </div>\n"
        "  );\n"
        "};\n");
    return code;
}

/* Image Optimization */
typedef struct {
    char *src;
    char *srcset;
    char *sizes;
    int lazy;
    int blur_placeholder;
    char *alt;
} OptimizedImage;

OptimizedImage* create_optimized_image(const char *src, const char *alt) {
    OptimizedImage *img = malloc(sizeof(OptimizedImage));
    img->src = strdup(src);
    img->srcset = NULL;
    img->sizes = NULL;
    img->lazy = 1;
    img->blur_placeholder = 1;
    img->alt = strdup(alt);
    return img;
}

char* generate_responsive_image(OptimizedImage *img) {
    char *code = malloc(2048);
    snprintf(code, 2048,
        "<img\n"
        "  src=\"%s\"\n"
        "  srcSet=\"%s\"\n"
        "  sizes=\"%s\"\n"
        "  alt=\"%s\"\n"
        "  loading=\"%s\"\n"
        "  decoding=\"async\"\n"
        "  style={{ width: '100%%', height: 'auto' }}\n"
        "/>",
        img->src,
        img->srcset ? img->srcset : img->src,
        img->sizes ? img->sizes : "(max-width: 768px) 100vw, 50vw",
        img->alt,
        img->lazy ? "lazy" : "eager");
    return code;
}

/* Bundle Size Optimization */
typedef struct {
    char *name;
    int size_bytes;
    int gzipped_size;
    char **imports;
    int import_count;
} BundleInfo;

BundleInfo* create_bundle_info(const char *name) {
    BundleInfo *info = malloc(sizeof(BundleInfo));
    info->name = strdup(name);
    info->size_bytes = 0;
    info->gzipped_size = 0;
    info->imports = NULL;
    info->import_count = 0;
    return info;
}

char* generate_tree_shaking_config() {
    char *config = malloc(2048);
    strcpy(config,
        "module.exports = {\n"
        "  optimization: {\n"
        "    usedExports: true,\n"
        "    sideEffects: false,\n"
        "    minimize: true,\n"
        "    splitChunks: {\n"
        "      chunks: 'all',\n"
        "      cacheGroups: {\n"
        "        vendor: {\n"
        "          test: /[\\\\/]node_modules[\\\\/]/,\n"
        "          name: 'vendors',\n"
        "          priority: 10\n"
        "        },\n"
        "        common: {\n"
        "          minChunks: 2,\n"
        "          priority: 5,\n"
        "          reuseExistingChunk: true\n"
        "        }\n"
        "      }\n"
        "    }\n"
        "  }\n"
        "};\n");
    return config;
}

/* Debounce and Throttle Utilities */
char* generate_debounce_utility(int delay_ms) {
    char *code = malloc(1024);
    snprintf(code, 1024,
        "const debounce = (func, delay = %d) => {\n"
        "  let timeoutId;\n"
        "  return (...args) => {\n"
        "    clearTimeout(timeoutId);\n"
        "    timeoutId = setTimeout(() => func(...args), delay);\n"
        "  };\n"
        "};\n",
        delay_ms);
    return code;
}

char* generate_throttle_utility(int delay_ms) {
    char *code = malloc(1024);
    snprintf(code, 1024,
        "const throttle = (func, delay = %d) => {\n"
        "  let lastCall = 0;\n"
        "  return (...args) => {\n"
        "    const now = Date.now();\n"
        "    if (now - lastCall >= delay) {\n"
        "      lastCall = now;\n"
        "      func(...args);\n"
        "    }\n"
        "  };\n"
        "};\n",
        delay_ms);
    return code;
}

/* Request Batching */
typedef struct BatchRequest {
    char *url;
    char *method;
    void *data;
    struct BatchRequest *next;
} BatchRequest;

typedef struct {
    BatchRequest *requests;
    int count;
    int max_batch_size;
    int batch_delay_ms;
} RequestBatcher;

RequestBatcher* create_request_batcher(int max_batch_size, int batch_delay_ms) {
    RequestBatcher *batcher = malloc(sizeof(RequestBatcher));
    batcher->requests = NULL;
    batcher->count = 0;
    batcher->max_batch_size = max_batch_size;
    batcher->batch_delay_ms = batch_delay_ms;
    return batcher;
}

char* generate_request_batcher() {
    char *code = malloc(2048);
    strcpy(code,
        "class RequestBatcher {\n"
        "  constructor(maxBatchSize = 10, delayMs = 50) {\n"
        "    this.queue = [];\n"
        "    this.maxBatchSize = maxBatchSize;\n"
        "    this.delayMs = delayMs;\n"
        "    this.timeoutId = null;\n"
        "  }\n"
        "  \n"
        "  add(request) {\n"
        "    this.queue.push(request);\n"
        "    \n"
        "    if (this.queue.length >= this.maxBatchSize) {\n"
        "      this.flush();\n"
        "    } else if (!this.timeoutId) {\n"
        "      this.timeoutId = setTimeout(() => this.flush(), this.delayMs);\n"
        "    }\n"
        "  }\n"
        "  \n"
        "  async flush() {\n"
        "    if (this.timeoutId) {\n"
        "      clearTimeout(this.timeoutId);\n"
        "      this.timeoutId = null;\n"
        "    }\n"
        "    \n"
        "    if (this.queue.length === 0) return;\n"
        "    \n"
        "    const batch = this.queue.splice(0, this.maxBatchSize);\n"
        "    \n"
        "    try {\n"
        "      const response = await fetch('/api/batch', {\n"
        "        method: 'POST',\n"
        "        headers: { 'Content-Type': 'application/json' },\n"
        "        body: JSON.stringify({ requests: batch })\n"
        "      });\n"
        "      return await response.json();\n"
        "    } catch (error) {\n"
        "      console.error('Batch request failed:', error);\n"
        "    }\n"
        "  }\n"
        "}\n");
    return code;
}

/* Service Worker for Caching */
char* generate_service_worker() {
    char *code = malloc(4096);
    strcpy(code,
        "const CACHE_NAME = 'app-cache-v1';\n"
        "const urlsToCache = [\n"
        "  '/',\n"
        "  '/static/css/main.css',\n"
        "  '/static/js/main.js'\n"
        "];\n"
        "\n"
        "self.addEventListener('install', (event) => {\n"
        "  event.waitUntil(\n"
        "    caches.open(CACHE_NAME)\n"
        "      .then((cache) => cache.addAll(urlsToCache))\n"
        "  );\n"
        "});\n"
        "\n"
        "self.addEventListener('fetch', (event) => {\n"
        "  event.respondWith(\n"
        "    caches.match(event.request)\n"
        "      .then((response) => {\n"
        "        if (response) return response;\n"
        "        \n"
        "        return fetch(event.request).then((response) => {\n"
        "          if (!response || response.status !== 200 || response.type !== 'basic') {\n"
        "            return response;\n"
        "          }\n"
        "          \n"
        "          const responseToCache = response.clone();\n"
        "          caches.open(CACHE_NAME)\n"
        "            .then((cache) => cache.put(event.request, responseToCache));\n"
        "          \n"
        "          return response;\n"
        "        });\n"
        "      })\n"
        "  );\n"
        "});\n"
        "\n"
        "self.addEventListener('activate', (event) => {\n"
        "  event.waitUntil(\n"
        "    caches.keys().then((cacheNames) => {\n"
        "      return Promise.all(\n"
        "        cacheNames.map((cacheName) => {\n"
        "          if (cacheName !== CACHE_NAME) {\n"
        "            return caches.delete(cacheName);\n"
        "          }\n"
        "        })\n"
        "      );\n"
        "    })\n"
        "  );\n"
        "});\n");
    return code;
}

/* Web Workers for Heavy Computation */
char* generate_web_worker(const char *worker_name) {
    char *code = malloc(2048);
    snprintf(code, 2048,
        "// %s.worker.js\n"
        "self.addEventListener('message', (e) => {\n"
        "  const { type, data } = e.data;\n"
        "  \n"
        "  switch(type) {\n"
        "    case 'PROCESS_DATA':\n"
        "      const result = processData(data);\n"
        "      self.postMessage({ type: 'RESULT', data: result });\n"
        "      break;\n"
        "    \n"
        "    case 'HEAVY_COMPUTATION':\n"
        "      const computed = heavyComputation(data);\n"
        "      self.postMessage({ type: 'COMPUTED', data: computed });\n"
        "      break;\n"
        "    \n"
        "    default:\n"
        "      console.warn('Unknown message type:', type);\n"
        "  }\n"
        "});\n"
        "\n"
        "function processData(data) {\n"
        "  // Heavy data processing logic\n"
        "  return data.map(item => item * 2);\n"
        "}\n"
        "\n"
        "function heavyComputation(data) {\n"
        "  // CPU-intensive computation\n"
        "  let result = 0;\n"
        "  for (let i = 0; i < data.length; i++) {\n"
        "    result += Math.sqrt(data[i]);\n"
        "  }\n"
        "  return result;\n"
        "}\n",
        worker_name);
    return code;
}

char* generate_worker_usage(const char *worker_name) {
    char *code = malloc(1024);
    snprintf(code, 1024,
        "const worker = new Worker('%s.worker.js');\n"
        "\n"
        "worker.postMessage({ type: 'PROCESS_DATA', data: [1, 2, 3, 4, 5] });\n"
        "\n"
        "worker.addEventListener('message', (e) => {\n"
        "  const { type, data } = e.data;\n"
        "  console.log('Worker result:', type, data);\n"
        "});\n",
        worker_name);
    return code;
}

/* Resource Hints */
char* generate_resource_hints(char **resources, int count) {
    char *html = malloc(4096);
    strcpy(html, "<!-- Resource Hints -->\n");
    
    for (int i = 0; i < count; i++) {
        char hint[512];
        snprintf(hint, sizeof(hint),
            "<link rel=\"preload\" href=\"%s\" as=\"script\">\n"
            "<link rel=\"prefetch\" href=\"%s\">\n"
            "<link rel=\"dns-prefetch\" href=\"%s\">\n",
            resources[i], resources[i], resources[i]);
        strcat(html, hint);
    }
    
    return html;
}

/* Critical CSS Extraction */
char* generate_critical_css_inline() {
    char *html = malloc(2048);
    strcpy(html,
        "<style>\n"
        "  /* Critical CSS - Above the fold */\n"
        "  body { margin: 0; font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', sans-serif; }\n"
        "  .header { background: #fff; padding: 16px; box-shadow: 0 2px 4px rgba(0,0,0,0.1); }\n"
        "  .hero { min-height: 400px; display: flex; align-items: center; justify-content: center; }\n"
        "  .container { max-width: 1200px; margin: 0 auto; padding: 0 16px; }\n"
        "</style>\n"
        "<link rel=\"preload\" href=\"/styles/main.css\" as=\"style\" onload=\"this.onload=null;this.rel='stylesheet'\">\n"
        "<noscript><link rel=\"stylesheet\" href=\"/styles/main.css\"></noscript>\n");
    return html;
}

/* Performance Monitoring */
char* generate_performance_monitoring() {
    char *code = malloc(4096);
    strcpy(code,
        "const measurePerformance = () => {\n"
        "  if ('performance' in window) {\n"
        "    const perfData = window.performance.timing;\n"
        "    const pageLoadTime = perfData.loadEventEnd - perfData.navigationStart;\n"
        "    const connectTime = perfData.responseEnd - perfData.requestStart;\n"
        "    const renderTime = perfData.domComplete - perfData.domLoading;\n"
        "    \n"
        "    console.log('Performance Metrics:');\n"
        "    console.log('Page Load Time:', pageLoadTime, 'ms');\n"
        "    console.log('Connect Time:', connectTime, 'ms');\n"
        "    console.log('Render Time:', renderTime, 'ms');\n"
        "    \n"
        "    // Core Web Vitals\n"
        "    if ('PerformanceObserver' in window) {\n"
        "      // Largest Contentful Paint (LCP)\n"
        "      new PerformanceObserver((list) => {\n"
        "        const entries = list.getEntries();\n"
        "        const lastEntry = entries[entries.length - 1];\n"
        "        console.log('LCP:', lastEntry.renderTime || lastEntry.loadTime);\n"
        "      }).observe({ entryTypes: ['largest-contentful-paint'] });\n"
        "      \n"
        "      // First Input Delay (FID)\n"
        "      new PerformanceObserver((list) => {\n"
        "        const entries = list.getEntries();\n"
        "        entries.forEach((entry) => {\n"
        "          console.log('FID:', entry.processingStart - entry.startTime);\n"
        "        });\n"
        "      }).observe({ entryTypes: ['first-input'] });\n"
        "      \n"
        "      // Cumulative Layout Shift (CLS)\n"
        "      let clsScore = 0;\n"
        "      new PerformanceObserver((list) => {\n"
        "        list.getEntries().forEach((entry) => {\n"
        "          if (!entry.hadRecentInput) {\n"
        "            clsScore += entry.value;\n"
        "          }\n"
        "        });\n"
        "        console.log('CLS:', clsScore);\n"
        "      }).observe({ entryTypes: ['layout-shift'] });\n"
        "    }\n"
        "  }\n"
        "};\n"
        "\n"
        "window.addEventListener('load', measurePerformance);\n");
    return code;
}

/* Intersection Observer for Lazy Loading */
char* generate_intersection_observer_lazy_load() {
    char *code = malloc(2048);
    strcpy(code,
        "const lazyLoadImages = () => {\n"
        "  const images = document.querySelectorAll('img[data-src]');\n"
        "  \n"
        "  const imageObserver = new IntersectionObserver((entries, observer) => {\n"
        "    entries.forEach(entry => {\n"
        "      if (entry.isIntersecting) {\n"
        "        const img = entry.target;\n"
        "        img.src = img.dataset.src;\n"
        "        img.removeAttribute('data-src');\n"
        "        imageObserver.unobserve(img);\n"
        "      }\n"
        "    });\n"
        "  }, {\n"
        "    rootMargin: '50px 0px',\n"
        "    threshold: 0.01\n"
        "  });\n"
        "  \n"
        "  images.forEach(img => imageObserver.observe(img));\n"
        "};\n"
        "\n"
        "document.addEventListener('DOMContentLoaded', lazyLoadImages);\n");
    return code;
}

/* Request Idle Callback for Non-Critical Tasks */
char* generate_idle_callback_usage() {
    char *code = malloc(1024);
    strcpy(code,
        "const scheduleNonCriticalWork = (task) => {\n"
        "  if ('requestIdleCallback' in window) {\n"
        "    requestIdleCallback((deadline) => {\n"
        "      while (deadline.timeRemaining() > 0 && tasks.length > 0) {\n"
        "        const task = tasks.shift();\n"
        "        task();\n"
        "      }\n"
        "    }, { timeout: 2000 });\n"
        "  } else {\n"
        "    setTimeout(task, 1);\n"
        "  }\n"
        "};\n");
    return code;
}
