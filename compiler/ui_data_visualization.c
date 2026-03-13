/* Data Visualization System for L# Language
 * Handles charts, graphs, data tables, dashboards, and visual analytics
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "ui_renderer.h"

/* Chart Types */
typedef enum {
    CHART_LINE,
    CHART_BAR,
    CHART_PIE,
    CHART_DOUGHNUT,
    CHART_AREA,
    CHART_SCATTER,
    CHART_BUBBLE,
    CHART_RADAR,
    CHART_POLAR,
    CHART_CANDLESTICK,
    CHART_HEATMAP,
    CHART_TREEMAP,
    CHART_SANKEY,
    CHART_FUNNEL,
    CHART_GAUGE,
    CHART_WATERFALL
} ChartType;

/* Data Series */
typedef struct DataPoint {
    double x;
    double y;
    double z;
    char *label;
    char *color;
    struct DataPoint *next;
} DataPoint;

typedef struct DataSeries {
    char *name;
    DataPoint *points;
    int point_count;
    char *color;
    int fill;
    float line_width;
    struct DataSeries *next;
} DataSeries;

DataSeries* create_data_series(const char *name, const char *color) {
    DataSeries *series = malloc(sizeof(DataSeries));
    series->name = strdup(name);
    series->points = NULL;
    series->point_count = 0;
    series->color = strdup(color);
    series->fill = 0;
    series->line_width = 2.0;
    series->next = NULL;
    return series;
}

void add_data_point(DataSeries *series, double x, double y, const char *label) {
    DataPoint *point = malloc(sizeof(DataPoint));
    point->x = x;
    point->y = y;
    point->z = 0;
    point->label = label ? strdup(label) : NULL;
    point->color = NULL;
    point->next = series->points;
    series->points = point;
    series->point_count++;
}

/* Chart Configuration */
typedef struct {
    char *title;
    char *x_axis_label;
    char *y_axis_label;
    int show_legend;
    int show_grid;
    int show_tooltips;
    int animate;
    int responsive;
    int width;
    int height;
} ChartConfig;

ChartConfig* create_chart_config() {
    ChartConfig *config = malloc(sizeof(ChartConfig));
    config->title = NULL;
    config->x_axis_label = NULL;
    config->y_axis_label = NULL;
    config->show_legend = 1;
    config->show_grid = 1;
    config->show_tooltips = 1;
    config->animate = 1;
    config->responsive = 1;
    config->width = 600;
    config->height = 400;
    return config;
}

/* Line Chart */
char* generate_line_chart(DataSeries *series, ChartConfig *config) {
    char *code = malloc(8192);
    strcpy(code, "{\n  type: 'line',\n  data: {\n    datasets: [\n");
    
    DataSeries *current = series;
    while (current) {
        char dataset[2048];
        snprintf(dataset, sizeof(dataset),
            "      {\n"
            "        label: '%s',\n"
            "        data: [",
            current->name);
        strcat(code, dataset);
        
        DataPoint *point = current->points;
        int first = 1;
        while (point) {
            char point_str[128];
            snprintf(point_str, sizeof(point_str),
                "%s{ x: %.2f, y: %.2f }",
                first ? "" : ", ",
                point->x, point->y);
            strcat(code, point_str);
            first = 0;
            point = point->next;
        }
        
        snprintf(dataset, sizeof(dataset),
            "],\n"
            "        borderColor: '%s',\n"
            "        backgroundColor: '%s',\n"
            "        borderWidth: %.1f,\n"
            "        fill: %s\n"
            "      }%s\n",
            current->color,
            current->color,
            current->line_width,
            current->fill ? "true" : "false",
            current->next ? "," : "");
        strcat(code, dataset);
        
        current = current->next;
    }
    
    strcat(code, "    ]\n  },\n  options: {\n");
    
    if (config->responsive) {
        strcat(code, "    responsive: true,\n");
    }
    
    if (config->show_legend) {
        strcat(code, "    plugins: {\n      legend: { display: true }\n    },\n");
    }
    
    if (config->show_grid) {
        strcat(code, "    scales: {\n      x: { grid: { display: true } },\n      y: { grid: { display: true } }\n    }\n");
    }
    
    strcat(code, "  }\n}");
    return code;
}

/* Bar Chart */
char* generate_bar_chart(DataSeries *series, ChartConfig *config, int horizontal) {
    char *code = malloc(8192);
    snprintf(code, 256, "{\n  type: '%s',\n  data: {\n    labels: [",
             horizontal ? "horizontalBar" : "bar");
    
    DataPoint *point = series->points;
    int first = 1;
    while (point) {
        char label[128];
        snprintf(label, sizeof(label), "%s'%s'",
                 first ? "" : ", ",
                 point->label ? point->label : "");
        strcat(code, label);
        first = 0;
        point = point->next;
    }
    
    strcat(code, "],\n    datasets: [\n");
    
    DataSeries *current = series;
    while (current) {
        char dataset[2048];
        snprintf(dataset, sizeof(dataset),
            "      {\n"
            "        label: '%s',\n"
            "        data: [",
            current->name);
        strcat(code, dataset);
        
        point = current->points;
        first = 1;
        while (point) {
            char val[32];
            snprintf(val, sizeof(val), "%s%.2f", first ? "" : ", ", point->y);
            strcat(code, val);
            first = 0;
            point = point->next;
        }
        
        snprintf(dataset, sizeof(dataset),
            "],\n"
            "        backgroundColor: '%s',\n"
            "        borderColor: '%s',\n"
            "        borderWidth: 1\n"
            "      }%s\n",
            current->color,
            current->color,
            current->next ? "," : "");
        strcat(code, dataset);
        
        current = current->next;
    }
    
    strcat(code, "    ]\n  },\n  options: {\n    responsive: true,\n    indexAxis: '");
    strcat(code, horizontal ? "y" : "x");
    strcat(code, "'\n  }\n}");
    
    return code;
}

/* Pie Chart */
char* generate_pie_chart(DataSeries *series, ChartConfig *config, int is_doughnut) {
    char *code = malloc(4096);
    snprintf(code, 256, "{\n  type: '%s',\n  data: {\n    labels: [",
             is_doughnut ? "doughnut" : "pie");
    
    DataPoint *point = series->points;
    int first = 1;
    while (point) {
        char label[128];
        snprintf(label, sizeof(label), "%s'%s'",
                 first ? "" : ", ",
                 point->label ? point->label : "");
        strcat(code, label);
        first = 0;
        point = point->next;
    }
    
    strcat(code, "],\n    datasets: [{\n      data: [");
    
    point = series->points;
    first = 1;
    while (point) {
        char val[32];
        snprintf(val, sizeof(val), "%s%.2f", first ? "" : ", ", point->y);
        strcat(code, val);
        first = 0;
        point = point->next;
    }
    
    strcat(code, "],\n      backgroundColor: [");
    
    const char *colors[] = {
        "#FF6384", "#36A2EB", "#FFCE56", "#4BC0C0", "#9966FF",
        "#FF9F40", "#FF6384", "#C9CBCF", "#4BC0C0", "#FF6384"
    };
    
    for (int i = 0; i < series->point_count; i++) {
        char color[32];
        snprintf(color, sizeof(color), "%s'%s'",
                 i > 0 ? ", " : "",
                 colors[i % 10]);
        strcat(code, color);
    }
    
    strcat(code, "]\n    }]\n  },\n  options: {\n    responsive: true\n  }\n}");
    return code;
}

/* Scatter Plot */
char* generate_scatter_plot(DataSeries *series, ChartConfig *config) {
    char *code = malloc(8192);
    strcpy(code, "{\n  type: 'scatter',\n  data: {\n    datasets: [\n");
    
    DataSeries *current = series;
    while (current) {
        char dataset[2048];
        snprintf(dataset, sizeof(dataset),
            "      {\n"
            "        label: '%s',\n"
            "        data: [",
            current->name);
        strcat(code, dataset);
        
        DataPoint *point = current->points;
        int first = 1;
        while (point) {
            char point_str[128];
            snprintf(point_str, sizeof(point_str),
                "%s{ x: %.2f, y: %.2f }",
                first ? "" : ", ",
                point->x, point->y);
            strcat(code, point_str);
            first = 0;
            point = point->next;
        }
        
        snprintf(dataset, sizeof(dataset),
            "],\n"
            "        backgroundColor: '%s',\n"
            "        pointRadius: 5\n"
            "      }%s\n",
            current->color,
            current->next ? "," : "");
        strcat(code, dataset);
        
        current = current->next;
    }
    
    strcat(code, "    ]\n  },\n  options: {\n    responsive: true,\n    scales: {\n      x: { type: 'linear', position: 'bottom' }\n    }\n  }\n}");
    return code;
}

/* Area Chart */
char* generate_area_chart(DataSeries *series, ChartConfig *config) {
    char *code = generate_line_chart(series, config);
    
    char *fill_pos = strstr(code, "fill: false");
    if (fill_pos) {
        memcpy(fill_pos, "fill: true ", 11);
    }
    
    return code;
}

/* Radar Chart */
char* generate_radar_chart(DataSeries *series, ChartConfig *config) {
    char *code = malloc(4096);
    strcpy(code, "{\n  type: 'radar',\n  data: {\n    labels: [");
    
    DataPoint *point = series->points;
    int first = 1;
    while (point) {
        char label[128];
        snprintf(label, sizeof(label), "%s'%s'",
                 first ? "" : ", ",
                 point->label ? point->label : "");
        strcat(code, label);
        first = 0;
        point = point->next;
    }
    
    strcat(code, "],\n    datasets: [\n");
    
    DataSeries *current = series;
    while (current) {
        char dataset[1024];
        snprintf(dataset, sizeof(dataset),
            "      {\n"
            "        label: '%s',\n"
            "        data: [",
            current->name);
        strcat(code, dataset);
        
        point = current->points;
        first = 1;
        while (point) {
            char val[32];
            snprintf(val, sizeof(val), "%s%.2f", first ? "" : ", ", point->y);
            strcat(code, val);
            first = 0;
            point = point->next;
        }
        
        snprintf(dataset, sizeof(dataset),
            "],\n"
            "        backgroundColor: 'rgba(%s, 0.2)',\n"
            "        borderColor: '%s',\n"
            "        pointBackgroundColor: '%s'\n"
            "      }%s\n",
            current->color,
            current->color,
            current->color,
            current->next ? "," : "");
        strcat(code, dataset);
        
        current = current->next;
    }
    
    strcat(code, "    ]\n  },\n  options: {\n    responsive: true\n  }\n}");
    return code;
}

/* Heatmap */
typedef struct {
    int rows;
    int cols;
    double **values;
    char **row_labels;
    char **col_labels;
    char *color_scheme;
} HeatmapData;

HeatmapData* create_heatmap(int rows, int cols) {
    HeatmapData *hm = malloc(sizeof(HeatmapData));
    hm->rows = rows;
    hm->cols = cols;
    hm->values = malloc(rows * sizeof(double*));
    for (int i = 0; i < rows; i++) {
        hm->values[i] = calloc(cols, sizeof(double));
    }
    hm->row_labels = malloc(rows * sizeof(char*));
    hm->col_labels = malloc(cols * sizeof(char*));
    hm->color_scheme = strdup("viridis");
    return hm;
}

char* generate_heatmap_svg(HeatmapData *hm, int cell_size) {
    char *svg = malloc(32768);
    int width = hm->cols * cell_size;
    int height = hm->rows * cell_size;
    
    snprintf(svg, 512,
        "<svg width=\"%d\" height=\"%d\" xmlns=\"http://www.w3.org/2000/svg\">\n",
        width, height);
    
    double min_val = hm->values[0][0];
    double max_val = hm->values[0][0];
    
    for (int i = 0; i < hm->rows; i++) {
        for (int j = 0; j < hm->cols; j++) {
            if (hm->values[i][j] < min_val) min_val = hm->values[i][j];
            if (hm->values[i][j] > max_val) max_val = hm->values[i][j];
        }
    }
    
    for (int i = 0; i < hm->rows; i++) {
        for (int j = 0; j < hm->cols; j++) {
            double normalized = (hm->values[i][j] - min_val) / (max_val - min_val);
            int intensity = (int)(normalized * 255);
            
            char rect[512];
            snprintf(rect, sizeof(rect),
                "  <rect x=\"%d\" y=\"%d\" width=\"%d\" height=\"%d\" "
                "fill=\"rgb(%d, %d, %d)\" stroke=\"white\" stroke-width=\"1\"/>\n",
                j * cell_size, i * cell_size, cell_size, cell_size,
                intensity, 0, 255 - intensity);
            strcat(svg, rect);
        }
    }
    
    strcat(svg, "</svg>");
    return svg;
}

/* Gauge Chart */
typedef struct {
    double value;
    double min;
    double max;
    char *label;
    char *color;
    int show_value;
} GaugeConfig;

char* generate_gauge_chart(GaugeConfig *config) {
    char *code = malloc(2048);
    
    double percentage = ((config->value - config->min) / (config->max - config->min)) * 100;
    double angle = (percentage / 100.0) * 180 - 90;
    
    snprintf(code, 2048,
        "<svg width=\"200\" height=\"120\" viewBox=\"0 0 200 120\">\n"
        "  <path d=\"M 20 100 A 80 80 0 0 1 180 100\" fill=\"none\" stroke=\"#e0e0e0\" stroke-width=\"20\"/>\n"
        "  <path d=\"M 20 100 A 80 80 0 0 1 %.2f %.2f\" fill=\"none\" stroke=\"%s\" stroke-width=\"20\"/>\n"
        "  <text x=\"100\" y=\"90\" text-anchor=\"middle\" font-size=\"24\" font-weight=\"bold\">%.1f</text>\n"
        "  <text x=\"100\" y=\"110\" text-anchor=\"middle\" font-size=\"12\">%s</text>\n"
        "</svg>",
        100 + 80 * cos(angle * M_PI / 180),
        100 + 80 * sin(angle * M_PI / 180),
        config->color,
        config->value,
        config->label);
    
    return code;
}

/* Sparkline */
char* generate_sparkline(double *values, int count, int width, int height) {
    char *svg = malloc(4096);
    
    double min_val = values[0];
    double max_val = values[0];
    for (int i = 1; i < count; i++) {
        if (values[i] < min_val) min_val = values[i];
        if (values[i] > max_val) max_val = values[i];
    }
    
    snprintf(svg, 256,
        "<svg width=\"%d\" height=\"%d\" xmlns=\"http://www.w3.org/2000/svg\">\n"
        "  <polyline fill=\"none\" stroke=\"#0074d9\" stroke-width=\"2\" points=\"",
        width, height);
    
    for (int i = 0; i < count; i++) {
        double x = (double)i / (count - 1) * width;
        double normalized = (values[i] - min_val) / (max_val - min_val);
        double y = height - (normalized * height);
        
        char point[64];
        snprintf(point, sizeof(point), "%.2f,%.2f ", x, y);
        strcat(svg, point);
    }
    
    strcat(svg, "\"/>\n</svg>");
    return svg;
}

/* Data Table with Sorting and Filtering */
typedef struct {
    char ***data;
    char **headers;
    int rows;
    int cols;
    int sortable;
    int filterable;
    int paginated;
    int page_size;
} DataTable;

DataTable* create_data_table(int rows, int cols) {
    DataTable *table = malloc(sizeof(DataTable));
    table->rows = rows;
    table->cols = cols;
    table->data = malloc(rows * sizeof(char**));
    for (int i = 0; i < rows; i++) {
        table->data[i] = malloc(cols * sizeof(char*));
    }
    table->headers = malloc(cols * sizeof(char*));
    table->sortable = 1;
    table->filterable = 1;
    table->paginated = 1;
    table->page_size = 10;
    return table;
}

UIElement* render_data_table(DataTable *table) {
    UIElement *container = create_element("div");
    add_attribute(container, "className=\"data-table-container\"");
    
    if (table->filterable) {
        UIElement *filter = create_element("input");
        add_attribute(filter, "type=\"text\"");
        add_attribute(filter, "placeholder=\"Filter...\"");
        add_attribute(filter, "className=\"table-filter\"");
        add_child(container, filter);
    }
    
    UIElement *table_elem = create_element("table");
    add_attribute(table_elem, "className=\"data-table\"");
    add_attribute(table_elem, "style=\"width: 100%; border-collapse: collapse;\"");
    
    UIElement *thead = create_element("thead");
    UIElement *header_row = create_element("tr");
    
    for (int i = 0; i < table->cols; i++) {
        UIElement *th = create_element("th");
        add_attribute(th, "style=\"padding: 12px; text-align: left; background: #f5f5f5; border-bottom: 2px solid #ddd; cursor: pointer;\"");
        
        if (table->sortable) {
            char attr[256];
            snprintf(attr, sizeof(attr), "onClick={() => sortTable(%d)}", i);
            add_attribute(th, attr);
        }
        
        add_child(th, create_text_node(table->headers[i]));
        add_child(header_row, th);
    }
    
    add_child(thead, header_row);
    add_child(table_elem, thead);
    
    UIElement *tbody = create_element("tbody");
    for (int i = 0; i < table->rows; i++) {
        UIElement *row = create_element("tr");
        add_attribute(row, "style=\"border-bottom: 1px solid #ddd;\"");
        
        for (int j = 0; j < table->cols; j++) {
            UIElement *td = create_element("td");
            add_attribute(td, "style=\"padding: 12px;\"");
            add_child(td, create_text_node(table->data[i][j]));
            add_child(row, td);
        }
        
        add_child(tbody, row);
    }
    
    add_child(table_elem, tbody);
    add_child(container, table_elem);
    
    if (table->paginated) {
        UIElement *pagination = create_element("div");
        add_attribute(pagination, "className=\"pagination\"");
        add_attribute(pagination, "style=\"margin-top: 20px; text-align: center;\"");
        add_child(container, pagination);
    }
    
    return container;
}

/* Progress Indicators */
UIElement* create_progress_bar(double percentage, const char *color) {
    UIElement *container = create_element("div");
    add_attribute(container, "style=\"width: 100%; height: 20px; background: #e0e0e0; border-radius: 10px; overflow: hidden;\"");
    
    UIElement *bar = create_element("div");
    char style[512];
    snprintf(style, sizeof(style),
        "height: 100%%; width: %.1f%%; background: %s; transition: width 0.3s ease;",
        percentage, color);
    
    char attr[1024];
    snprintf(attr, sizeof(attr), "style=\"%s\"", style);
    add_attribute(bar, attr);
    
    add_child(container, bar);
    return container;
}

UIElement* create_circular_progress(double percentage, const char *color) {
    UIElement *svg = create_element("svg");
    add_attribute(svg, "width=\"100\"");
    add_attribute(svg, "height=\"100\"");
    add_attribute(svg, "viewBox=\"0 0 100 100\"");
    
    double circumference = 2 * M_PI * 40;
    double offset = circumference - (percentage / 100.0) * circumference;
    
    UIElement *circle_bg = create_element("circle");
    add_attribute(circle_bg, "cx=\"50\"");
    add_attribute(circle_bg, "cy=\"50\"");
    add_attribute(circle_bg, "r=\"40\"");
    add_attribute(circle_bg, "fill=\"none\"");
    add_attribute(circle_bg, "stroke=\"#e0e0e0\"");
    add_attribute(circle_bg, "stroke-width=\"8\"");
    add_child(svg, circle_bg);
    
    UIElement *circle = create_element("circle");
    add_attribute(circle, "cx=\"50\"");
    add_attribute(circle, "cy=\"50\"");
    add_attribute(circle, "r=\"40\"");
    add_attribute(circle, "fill=\"none\"");
    
    char attr[512];
    snprintf(attr, sizeof(attr), "stroke=\"%s\"", color);
    add_attribute(circle, attr);
    add_attribute(circle, "stroke-width=\"8\"");
    add_attribute(circle, "stroke-linecap=\"round\"");
    
    snprintf(attr, sizeof(attr), "stroke-dasharray=\"%.2f\"", circumference);
    add_attribute(circle, attr);
    
    snprintf(attr, sizeof(attr), "stroke-dashoffset=\"%.2f\"", offset);
    add_attribute(circle, attr);
    
    add_attribute(circle, "transform=\"rotate(-90 50 50)\"");
    add_child(svg, circle);
    
    return svg;
}
