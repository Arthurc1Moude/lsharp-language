/* String Algorithms for Symbol Table and Lexical Analysis */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

/* KMP Pattern Matching */
void compute_lps_array(char *pattern, int m, int *lps) {
    int len = 0;
    lps[0] = 0;
    int i = 1;
    
    while (i < m) {
        if (pattern[i] == pattern[len]) {
            len++;
            lps[i] = len;
            i++;
        } else {
            if (len != 0) {
                len = lps[len - 1];
            } else {
                lps[i] = 0;
                i++;
            }
        }
    }
}

int* kmp_search(char *text, char *pattern, int *count) {
    int n = strlen(text);
    int m = strlen(pattern);
    
    int *lps = malloc(m * sizeof(int));
    compute_lps_array(pattern, m, lps);
    
    int *matches = malloc(n * sizeof(int));
    *count = 0;
    
    int i = 0, j = 0;
    while (i < n) {
        if (pattern[j] == text[i]) {
            i++;
            j++;
        }
        
        if (j == m) {
            matches[(*count)++] = i - j;
            j = lps[j - 1];
        } else if (i < n && pattern[j] != text[i]) {
            if (j != 0) {
                j = lps[j - 1];
            } else {
                i++;
            }
        }
    }
    
    free(lps);
    return matches;
}

/* Rabin-Karp Algorithm */
#define PRIME 101
#define BASE 256

int* rabin_karp_search(char *text, char *pattern, int *count) {
    int n = strlen(text);
    int m = strlen(pattern);
    
    int *matches = malloc(n * sizeof(int));
    *count = 0;
    
    long long pattern_hash = 0;
    long long text_hash = 0;
    long long h = 1;
    
    for (int i = 0; i < m - 1; i++) {
        h = (h * BASE) % PRIME;
    }
    
    for (int i = 0; i < m; i++) {
        pattern_hash = (BASE * pattern_hash + pattern[i]) % PRIME;
        text_hash = (BASE * text_hash + text[i]) % PRIME;
    }
    
    for (int i = 0; i <= n - m; i++) {
        if (pattern_hash == text_hash) {
            int j;
            for (j = 0; j < m; j++) {
                if (text[i + j] != pattern[j]) {
                    break;
                }
            }
            if (j == m) {
                matches[(*count)++] = i;
            }
        }
        
        if (i < n - m) {
            text_hash = (BASE * (text_hash - text[i] * h) + text[i + m]) % PRIME;
            if (text_hash < 0) {
                text_hash += PRIME;
            }
        }
    }
    
    return matches;
}

/* Boyer-Moore Algorithm */
void compute_bad_char(char *pattern, int m, int *bad_char) {
    for (int i = 0; i < 256; i++) {
        bad_char[i] = -1;
    }
    
    for (int i = 0; i < m; i++) {
        bad_char[(int)pattern[i]] = i;
    }
}

int* boyer_moore_search(char *text, char *pattern, int *count) {
    int n = strlen(text);
    int m = strlen(pattern);
    
    int *bad_char = malloc(256 * sizeof(int));
    compute_bad_char(pattern, m, bad_char);
    
    int *matches = malloc(n * sizeof(int));
    *count = 0;
    
    int s = 0;
    while (s <= n - m) {
        int j = m - 1;
        
        while (j >= 0 && pattern[j] == text[s + j]) {
            j--;
        }
        
        if (j < 0) {
            matches[(*count)++] = s;
            s += (s + m < n) ? m - bad_char[(int)text[s + m]] : 1;
        } else {
            int shift = j - bad_char[(int)text[s + j]];
            s += (shift > 1) ? shift : 1;
        }
    }
    
    free(bad_char);
    return matches;
}

/* Aho-Corasick Algorithm for Multiple Pattern Matching */
#define MAX_CHARS 256

typedef struct ACNode {
    struct ACNode *children[MAX_CHARS];
    struct ACNode *failure;
    int *output;
    int output_count;
} ACNode;

ACNode* create_ac_node() {
    ACNode *node = malloc(sizeof(ACNode));
    for (int i = 0; i < MAX_CHARS; i++) {
        node->children[i] = NULL;
    }
    node->failure = NULL;
    node->output = NULL;
    node->output_count = 0;
    return node;
}

void ac_add_pattern(ACNode *root, char *pattern, int pattern_id) {
    ACNode *current = root;
    int len = strlen(pattern);
    
    for (int i = 0; i < len; i++) {
        int ch = (int)pattern[i];
        if (!current->children[ch]) {
            current->children[ch] = create_ac_node();
        }
        current = current->children[ch];
    }
    
    if (!current->output) {
        current->output = malloc(10 * sizeof(int));
    }
    current->output[current->output_count++] = pattern_id;
}

void ac_build_failure(ACNode *root) {
    ACNode **queue = malloc(10000 * sizeof(ACNode*));
    int front = 0, rear = 0;
    
    for (int i = 0; i < MAX_CHARS; i++) {
        if (root->children[i]) {
            root->children[i]->failure = root;
            queue[rear++] = root->children[i];
        }
    }
    
    while (front < rear) {
        ACNode *current = queue[front++];
        
        for (int i = 0; i < MAX_CHARS; i++) {
            if (current->children[i]) {
                ACNode *child = current->children[i];
                queue[rear++] = child;
                
                ACNode *f = current->failure;
                while (f && !f->children[i]) {
                    f = f->failure;
                }
                
                child->failure = f ? f->children[i] : root;
                
                if (child->failure->output_count > 0) {
                    if (!child->output) {
                        child->output = malloc(10 * sizeof(int));
                    }
                    for (int j = 0; j < child->failure->output_count; j++) {
                        child->output[child->output_count++] = child->failure->output[j];
                    }
                }
            }
        }
    }
    
    free(queue);
}

void ac_search(ACNode *root, char *text, int **results, int *result_count) {
    int n = strlen(text);
    ACNode *current = root;
    
    for (int i = 0; i < n; i++) {
        int ch = (int)text[i];
        
        while (current && !current->children[ch]) {
            current = current->failure;
        }
        
        current = current ? current->children[ch] : root;
        
        if (current->output_count > 0) {
            for (int j = 0; j < current->output_count; j++) {
                results[*result_count] = malloc(2 * sizeof(int));
                results[*result_count][0] = i;
                results[*result_count][1] = current->output[j];
                (*result_count)++;
            }
        }
    }
}

/* Suffix Array Construction */
typedef struct {
    int index;
    int rank[2];
} Suffix;

int compare_suffix(const void *a, const void *b) {
    Suffix *sa = (Suffix*)a;
    Suffix *sb = (Suffix*)b;
    
    if (sa->rank[0] != sb->rank[0]) {
        return sa->rank[0] - sb->rank[0];
    }
    return sa->rank[1] - sb->rank[1];
}

int* build_suffix_array(char *text, int n) {
    Suffix *suffixes = malloc(n * sizeof(Suffix));
    
    for (int i = 0; i < n; i++) {
        suffixes[i].index = i;
        suffixes[i].rank[0] = text[i];
        suffixes[i].rank[1] = (i + 1 < n) ? text[i + 1] : -1;
    }
    
    qsort(suffixes, n, sizeof(Suffix), compare_suffix);
    
    int *ind = malloc(n * sizeof(int));
    
    for (int k = 4; k < 2 * n; k *= 2) {
        int rank = 0;
        int prev_rank = suffixes[0].rank[0];
        suffixes[0].rank[0] = rank;
        ind[suffixes[0].index] = 0;
        
        for (int i = 1; i < n; i++) {
            if (suffixes[i].rank[0] == prev_rank &&
                suffixes[i].rank[1] == suffixes[i - 1].rank[1]) {
                prev_rank = suffixes[i].rank[0];
                suffixes[i].rank[0] = rank;
            } else {
                prev_rank = suffixes[i].rank[0];
                suffixes[i].rank[0] = ++rank;
            }
            ind[suffixes[i].index] = i;
        }
        
        for (int i = 0; i < n; i++) {
            int next_index = suffixes[i].index + k / 2;
            suffixes[i].rank[1] = (next_index < n) ? suffixes[ind[next_index]].rank[0] : -1;
        }
        
        qsort(suffixes, n, sizeof(Suffix), compare_suffix);
    }
    
    int *suffix_array = malloc(n * sizeof(int));
    for (int i = 0; i < n; i++) {
        suffix_array[i] = suffixes[i].index;
    }
    
    free(suffixes);
    free(ind);
    
    return suffix_array;
}

/* LCP Array Construction */
int* build_lcp_array(char *text, int *suffix_array, int n) {
    int *lcp = malloc(n * sizeof(int));
    int *inv_suffix = malloc(n * sizeof(int));
    
    for (int i = 0; i < n; i++) {
        inv_suffix[suffix_array[i]] = i;
    }
    
    int k = 0;
    for (int i = 0; i < n; i++) {
        if (inv_suffix[i] == n - 1) {
            k = 0;
            continue;
        }
        
        int j = suffix_array[inv_suffix[i] + 1];
        
        while (i + k < n && j + k < n && text[i + k] == text[j + k]) {
            k++;
        }
        
        lcp[inv_suffix[i]] = k;
        
        if (k > 0) {
            k--;
        }
    }
    
    free(inv_suffix);
    return lcp;
}

/* Longest Common Substring using Suffix Array */
int longest_common_substring(char *s1, char *s2) {
    int n1 = strlen(s1);
    int n2 = strlen(s2);
    
    char *combined = malloc((n1 + n2 + 2) * sizeof(char));
    sprintf(combined, "%s#%s", s1, s2);
    int n = strlen(combined);
    
    int *suffix_array = build_suffix_array(combined, n);
    int *lcp = build_lcp_array(combined, suffix_array, n);
    
    int max_lcp = 0;
    for (int i = 0; i < n - 1; i++) {
        int pos1 = suffix_array[i];
        int pos2 = suffix_array[i + 1];
        
        if ((pos1 < n1 && pos2 > n1) || (pos1 > n1 && pos2 < n1)) {
            if (lcp[i] > max_lcp) {
                max_lcp = lcp[i];
            }
        }
    }
    
    free(combined);
    free(suffix_array);
    free(lcp);
    
    return max_lcp;
}

/* Z-Algorithm */
int* z_algorithm(char *text) {
    int n = strlen(text);
    int *z = calloc(n, sizeof(int));
    
    int l = 0, r = 0;
    for (int i = 1; i < n; i++) {
        if (i > r) {
            l = r = i;
            while (r < n && text[r - l] == text[r]) {
                r++;
            }
            z[i] = r - l;
            r--;
        } else {
            int k = i - l;
            if (z[k] < r - i + 1) {
                z[i] = z[k];
            } else {
                l = i;
                while (r < n && text[r - l] == text[r]) {
                    r++;
                }
                z[i] = r - l;
                r--;
            }
        }
    }
    
    return z;
}

/* Manacher's Algorithm for Longest Palindromic Substring */
char* preprocess_string(char *s) {
    int n = strlen(s);
    char *processed = malloc((2 * n + 3) * sizeof(char));
    
    processed[0] = '^';
    for (int i = 0; i < n; i++) {
        processed[2 * i + 1] = '#';
        processed[2 * i + 2] = s[i];
    }
    processed[2 * n + 1] = '#';
    processed[2 * n + 2] = '$';
    processed[2 * n + 3] = '\0';
    
    return processed;
}

char* longest_palindrome(char *s) {
    char *processed = preprocess_string(s);
    int n = strlen(processed);
    
    int *p = calloc(n, sizeof(int));
    int center = 0, right = 0;
    
    for (int i = 1; i < n - 1; i++) {
        int mirror = 2 * center - i;
        
        if (i < right) {
            p[i] = (right - i < p[mirror]) ? (right - i) : p[mirror];
        }
        
        while (processed[i + p[i] + 1] == processed[i - p[i] - 1]) {
            p[i]++;
        }
        
        if (i + p[i] > right) {
            center = i;
            right = i + p[i];
        }
    }
    
    int max_len = 0, center_index = 0;
    for (int i = 1; i < n - 1; i++) {
        if (p[i] > max_len) {
            max_len = p[i];
            center_index = i;
        }
    }
    
    int start = (center_index - max_len) / 2;
    char *result = malloc((max_len + 1) * sizeof(char));
    strncpy(result, s + start, max_len);
    result[max_len] = '\0';
    
    free(processed);
    free(p);
    
    return result;
}

/* Trie Data Structure */
typedef struct TrieNode {
    struct TrieNode *children[26];
    int is_end_of_word;
    int frequency;
} TrieNode;

TrieNode* create_trie_node() {
    TrieNode *node = malloc(sizeof(TrieNode));
    node->is_end_of_word = 0;
    node->frequency = 0;
    for (int i = 0; i < 26; i++) {
        node->children[i] = NULL;
    }
    return node;
}

void trie_insert(TrieNode *root, char *word) {
    TrieNode *current = root;
    int len = strlen(word);
    
    for (int i = 0; i < len; i++) {
        int index = word[i] - 'a';
        if (!current->children[index]) {
            current->children[index] = create_trie_node();
        }
        current = current->children[index];
    }
    
    current->is_end_of_word = 1;
    current->frequency++;
}

int trie_search(TrieNode *root, char *word) {
    TrieNode *current = root;
    int len = strlen(word);
    
    for (int i = 0; i < len; i++) {
        int index = word[i] - 'a';
        if (!current->children[index]) {
            return 0;
        }
        current = current->children[index];
    }
    
    return current && current->is_end_of_word;
}

int trie_starts_with(TrieNode *root, char *prefix) {
    TrieNode *current = root;
    int len = strlen(prefix);
    
    for (int i = 0; i < len; i++) {
        int index = prefix[i] - 'a';
        if (!current->children[index]) {
            return 0;
        }
        current = current->children[index];
    }
    
    return 1;
}

/* Edit Distance (Levenshtein Distance) */
int min3(int a, int b, int c) {
    int min = a;
    if (b < min) min = b;
    if (c < min) min = c;
    return min;
}

int edit_distance(char *s1, char *s2) {
    int m = strlen(s1);
    int n = strlen(s2);
    
    int **dp = malloc((m + 1) * sizeof(int*));
    for (int i = 0; i <= m; i++) {
        dp[i] = malloc((n + 1) * sizeof(int));
    }
    
    for (int i = 0; i <= m; i++) {
        dp[i][0] = i;
    }
    for (int j = 0; j <= n; j++) {
        dp[0][j] = j;
    }
    
    for (int i = 1; i <= m; i++) {
        for (int j = 1; j <= n; j++) {
            if (s1[i - 1] == s2[j - 1]) {
                dp[i][j] = dp[i - 1][j - 1];
            } else {
                dp[i][j] = 1 + min3(dp[i - 1][j], dp[i][j - 1], dp[i - 1][j - 1]);
            }
        }
    }
    
    int result = dp[m][n];
    
    for (int i = 0; i <= m; i++) {
        free(dp[i]);
    }
    free(dp);
    
    return result;
}

/* Longest Common Subsequence */
int lcs_length(char *s1, char *s2) {
    int m = strlen(s1);
    int n = strlen(s2);
    
    int **dp = malloc((m + 1) * sizeof(int*));
    for (int i = 0; i <= m; i++) {
        dp[i] = calloc(n + 1, sizeof(int));
    }
    
    for (int i = 1; i <= m; i++) {
        for (int j = 1; j <= n; j++) {
            if (s1[i - 1] == s2[j - 1]) {
                dp[i][j] = dp[i - 1][j - 1] + 1;
            } else {
                dp[i][j] = (dp[i - 1][j] > dp[i][j - 1]) ? dp[i - 1][j] : dp[i][j - 1];
            }
        }
    }
    
    int result = dp[m][n];
    
    for (int i = 0; i <= m; i++) {
        free(dp[i]);
    }
    free(dp);
    
    return result;
}

/* String Hashing */
unsigned long long polynomial_hash(char *s) {
    unsigned long long hash = 0;
    unsigned long long p = 31;
    unsigned long long p_pow = 1;
    int n = strlen(s);
    
    for (int i = 0; i < n; i++) {
        hash += (s[i] - 'a' + 1) * p_pow;
        p_pow *= p;
    }
    
    return hash;
}

/* Rolling Hash */
typedef struct {
    unsigned long long *hash;
    unsigned long long *p_pow;
    int n;
    unsigned long long p;
} RollingHash;

RollingHash* create_rolling_hash(char *s) {
    RollingHash *rh = malloc(sizeof(RollingHash));
    rh->n = strlen(s);
    rh->p = 31;
    rh->hash = malloc((rh->n + 1) * sizeof(unsigned long long));
    rh->p_pow = malloc((rh->n + 1) * sizeof(unsigned long long));
    
    rh->hash[0] = 0;
    rh->p_pow[0] = 1;
    
    for (int i = 0; i < rh->n; i++) {
        rh->hash[i + 1] = rh->hash[i] + (s[i] - 'a' + 1) * rh->p_pow[i];
        rh->p_pow[i + 1] = rh->p_pow[i] * rh->p;
    }
    
    return rh;
}

unsigned long long get_substring_hash(RollingHash *rh, int l, int r) {
    return (rh->hash[r] - rh->hash[l]) / rh->p_pow[l];
}
