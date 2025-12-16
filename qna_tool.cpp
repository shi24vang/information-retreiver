#include <assert.h>
#include <cstdlib>
#include <sstream>
#include "qna_tool.h"

using namespace std;

template <typename T>
inline void swap_items(T& a, T& b) {
    T tmp = a;
    a = b;
    b = tmp;
}

template <class T, class X>
class AVLMap {
    struct Node {
        T key;
        X val;
        int height;
        Node* left;
        Node* right;
        Node(T k, X v) : key(k), val(v), height(0), left(nullptr), right(nullptr) {}
        ~Node() {
            delete left;
            delete right;
        }
    };

    Node* root;

    static int h(Node* n) {
        return n ? n->height : -1;
    }

    static void pull(Node* n) {
        if (n) n->height = std::max(h(n->left), h(n->right)) + 1;
    }

    static Node* rotate_left(Node* n) {
        Node* p = n->left;
        n->left = p->right;
        p->right = n;
        pull(n);
        pull(p);
        return p;
    }

    static Node* rotate_right(Node* n) {
        Node* p = n->right;
        n->right = p->left;
        p->left = n;
        pull(n);
        pull(p);
        return p;
    }

    static Node* rebalance(Node* n) {
        if (!n) return n;
        int diff = h(n->left) - h(n->right);
        if (abs(diff) < 2) return n;
        if (diff > 0) {
            if (h(n->left->right) > h(n->left->left)) n->left = rotate_right(n->left);
            return rotate_left(n);
        } else {
            if (h(n->right->left) > h(n->right->right)) n->right = rotate_left(n->right);
            return rotate_right(n);
        }
    }

    static Node* insert_rec(Node* n, T key, X val) {
        if (!n) return new Node(key, val);
        if (key < n->key) {
            n->left = insert_rec(n->left, key, val);
        } else {
            n->right = insert_rec(n->right, key, val);
        }
        pull(n);
        return rebalance(n);
    }

    static Node* find_rec(Node* n, const T& key) {
        if (!n) return nullptr;
        if (n->key == key) return n;
        return key < n->key ? find_rec(n->left, key) : find_rec(n->right, key);
    }

    static void collect(Node* n, vector<pair<T, X>>& out) {
        if (!n) return;
        collect(n->left, out);
        out.push_back({n->key, n->val});
        collect(n->right, out);
    }

public:
    AVLMap() : root(nullptr) {}
    ~AVLMap() { delete root; }

    void insert(T key, X val) {
        Node* existing = find_rec(root, key);
        if (existing) {
            existing->val = val;
        } else {
            root = insert_rec(root, key, val);
        }
    }

    bool exist(T key) {
        return find_rec(root, key) != nullptr;
    }

    Node* find(T key) {
        return find_rec(root, key);
    }

    X get(T key) {
        Node* n = find_rec(root, key);
        if (!n) return X();
        return n->val;
    }

    void increase_by_x(T key, X delta) {
        Node* n = find_rec(root, key);
        if (!n) {
            insert(key, delta);
        } else {
            n->val += delta;
        }
    }

    void get_all(vector<pair<T, X>>& out) {
        collect(root, out);
    }
};

template <class t, class d>
class Tries {
public:
    long long total;
    long long c_val;
    AVLMap<char, Tries*> child;
    AVLMap<t, d> data;
    Tries() : total(0), c_val(0) {}

    void insert(string word, long long c, int idx) {
        if (idx == static_cast<int>(word.size())) {
            c_val = c;
            return;
        }
        Tries* nxt = child.get(word[idx]);
        if (!nxt) {
            child.insert(word[idx], new Tries());
            nxt = child.get(word[idx]);
        }
        nxt->insert(word, c, idx + 1);
    }

    void insert(string word, t key, d val, int idx) {
        if (idx == static_cast<int>(word.size())) {
            data.insert(key, val);
            return;
        }
        Tries* nxt = child.get(word[idx]);
        if (!nxt) {
            child.insert(word[idx], new Tries());
            nxt = child.get(word[idx]);
        }
        nxt->insert(word, key, val, idx + 1);
    }

    Tries* get(string word, int idx) {
        if (idx == static_cast<int>(word.size())) return this;
        Tries* nxt = child.get(word[idx]);
        if (!nxt) return nullptr;
        return nxt->get(word, idx + 1);
    }

    void increase_by_1(string word, t key, int idx) {
        if (idx >= static_cast<int>(word.size())) {
            total++;
            if (!data.exist(key)) {
                data.insert(key, 1);
            } else {
                data.increase_by_x(key, 1);
            }
            return;
        }
        Tries* nxt = child.get(word[idx]);
        if (!nxt) {
            child.insert(word[idx], new Tries());
            nxt = child.get(word[idx]);
        }
        nxt->increase_by_1(word, key, idx + 1);
    }
};

template <class T>
class Heap {
    vector<T> store;

    static size_t parent(size_t i) { return (i - 1) / 2; }
    static size_t left(size_t i) { return 2 * i + 1; }
    static size_t right(size_t i) { return 2 * i + 2; }

    void sift_up(size_t idx) {
        while (idx > 0 && store[parent(idx)] > store[idx]) {
            swap_items(store[parent(idx)], store[idx]);
            idx = parent(idx);
        }
    }

    void sift_down(size_t idx) {
        while (left(idx) < store.size()) {
            size_t smallest = left(idx);
            if (right(idx) < store.size() && store[right(idx)] < store[smallest]) smallest = right(idx);
            if (!(store[smallest] < store[idx])) break;
            swap_items(store[smallest], store[idx]);
            idx = smallest;
        }
    }

public:
    void insert(T value) {
        store.push_back(value);
        sift_up(store.size() - 1);
    }

    void pop() {
        if (store.empty()) return;
        store[0] = store.back();
        store.pop_back();
        if (!store.empty()) sift_down(0);
    }

    T get_top() const {
        if (store.empty()) return T();
        return store.front();
    }

    size_t get_size() const {
        return store.size();
    }
};

static void merge_strings(vector<string>& arr, int l, int r) {
    if (l >= r) return;
    int m = (l + r) / 2;
    merge_strings(arr, l, m);
    merge_strings(arr, m + 1, r);
    vector<string> buf;
    int i = l, j = m + 1;
    while (i <= m && j <= r) {
        if (arr[i] < arr[j]) buf.push_back(arr[i++]);
        else buf.push_back(arr[j++]);
    }
    while (i <= m) buf.push_back(arr[i++]);
    while (j <= r) buf.push_back(arr[j++]);
    for (int k = l; k <= r; ++k) arr[k] = buf[k - l];
}

static bool contains_word(const vector<string>& words, const string& key) {
    for (const auto& item : words) {
        if (item == key) return true;
    }
    return false;
}

static vector<pair<string, int>> rake(const string& sentence) {
    vector<string> stops = {"a", "about", "above", "after", "again", "all", "am", "an", "and",
                             "are", "as", "at", "be", "because", "been", "before", "both", "but", "by",
                             "can't", "did", "do", "does", "don't", "down", "during", "each", "for", "from", "gandhi", "gandhis",
                             "had", "has", "have", "he", "here", "hers", "herself", "him", "himself", "his", "how",
                             "i", "if", "in", "into", "is", "it", "its", "itself", "let's", "mahatma", "mahatmas", "my",
                             "no", "nor", "not", "of", "on", "or", "our", "ours", "ourselves", "out",
                             "over", "own", "same", "she", "should", "so", "some", "than", "that", "the", "their",
                             "them", "themselves", "then", "there", "these", "they", "this", "through", "to", "too",
                             "under", "until", "up", "very", "vol", "was", "we", "were", "what", "when", "where", "which",
                             "while", "who", "why", "will", "with", "you", "your", "yours", "yourself", "yourselves"};
    vector<string> tokens;
    string cur;
    string separators = " ,.?!:;\"'[]{}()";
    for (char ch : sentence) {
        if (separators.find(ch) != string::npos) {
            if (!cur.empty()) {
                if (!contains_word(stops, cur)) tokens.push_back(cur);
                cur.clear();
            }
        } else {
            cur.push_back(ch >= 'A' && ch <= 'Z' ? static_cast<char>(ch - 'A' + 'a') : ch);
        }
    }
    if (!cur.empty() && !contains_word(stops, cur)) tokens.push_back(cur);
    if (tokens.empty()) return {};
    merge_strings(tokens, 0, tokens.size() - 1);
    vector<pair<string, int>> res;
    res.push_back({tokens[0], 1});
    for (size_t i = 1; i < tokens.size(); ++i) {
        if (tokens[i] == tokens[i - 1]) {
            res.back().second++;
        } else {
            res.push_back({tokens[i], 1});
        }
    }
    return res;
}

struct Graph_Node {
    int book_code;
    int page;
    int paragraph;
    int total_words;
    vector<pair<string, int>> words;
};

struct Graph {
    vector<Graph_Node*> nodes;

    Graph_Node* locate(int b, int p, int par) {
        for (auto node : nodes) {
            if (node->book_code == b && node->page == p && node->paragraph == par) return node;
        }
        return nullptr;
    }

    void add_node(int book_code, int page, int paragraph, const pair<string, int>& word, int total_words) {
        Graph_Node* existing = locate(book_code, page, paragraph);
        if (!existing) {
            Graph_Node* node = new Graph_Node();
            node->book_code = book_code;
            node->page = page;
            node->paragraph = paragraph;
            node->total_words = total_words;
            node->words.push_back(word);
            nodes.push_back(node);
        } else {
            existing->words.push_back(word);
        }
    }

    int compare(const vector<pair<string, int>>& a, const vector<pair<string, int>>& b) const {
        int i = 0, j = 0;
        int score = 0;
        while (i < static_cast<int>(a.size()) && j < static_cast<int>(b.size())) {
            if (a[i].first == b[j].first) {
                score += a[i].second;
                i++;
                j++;
            } else if (a[i].first < b[j].first) {
                score += 3 * a[i].second;
                i++;
            } else {
                j++;
            }
        }
        while (i < static_cast<int>(a.size())) {
            score += 3 * a[i].second;
            i++;
        }
        return score;
    }

    vector<pair<pair<int, pair<int, int>>, double>> get_score() {
        vector<pair<pair<int, pair<int, int>>, double>> ans;
        if (nodes.empty()) return ans;
        vector<double> score(nodes.size(), 1.0 / nodes.size());
        vector<vector<double>> edges(nodes.size(), vector<double>(nodes.size(), 0));
        for (int i = 0; i < static_cast<int>(nodes.size()); ++i) {
            for (int j = 0; j < static_cast<int>(nodes.size()); ++j) {
                edges[i][j] = compare(nodes[j]->words, nodes[i]->words) / (nodes[j]->total_words + 1.0);
            }
        }
        for (int i = 0; i < static_cast<int>(nodes.size()); ++i) {
            double sum = 0;
            for (double val : edges[i]) sum += val;
            if (sum != 0) {
                for (double& val : edges[i]) val /= sum;
            }
        }
        for (int iter = 0; iter < 10; ++iter) {
            vector<double> next(nodes.size(), 0);
            for (int i = 0; i < static_cast<int>(nodes.size()); ++i) {
                for (int j = 0; j < static_cast<int>(nodes.size()); ++j) {
                    next[j] += score[i] * edges[i][j];
                }
            }
            score.swap(next);
        }
        for (int i = 0; i < static_cast<int>(nodes.size()); ++i) {
            ans.push_back({{nodes[i]->book_code, {nodes[i]->page, nodes[i]->paragraph}}, score[i]});
        }
        return ans;
    }
};

static void merge_scores(vector<pair<pair<int, pair<int, int>>, double>>& arr, int l, int r) {
    if (l >= r) return;
    int m = (l + r) / 2;
    merge_scores(arr, l, m);
    merge_scores(arr, m + 1, r);
    vector<pair<pair<int, pair<int, int>>, double>> tmp;
    int i = l, j = m + 1;
    while (i <= m && j <= r) {
        if (arr[i].second > arr[j].second) tmp.push_back(arr[i++]);
        else tmp.push_back(arr[j++]);
    }
    while (i <= m) tmp.push_back(arr[i++]);
    while (j <= r) tmp.push_back(arr[j++]);
    for (int k = l; k <= r; ++k) arr[k] = tmp[k - l];
}

static Node* gather_top(const vector<pair<pair<int, pair<int, int>>, double>>& scores, QNA_tool& q, int& k_out) {
    Node* head = nullptr;
    int words_used = 0;
    k_out = 0;
    for (auto entry : scores) {
        int cost = q.counter->get(entry.first);
        if (words_used + cost > 2000) continue;
        Node* node = new Node();
        node->book_code = entry.first.first;
        node->page = entry.first.second.first;
        node->paragraph = entry.first.second.second;
        node->right = head;
        if (head) head->left = node;
        head = node;
        words_used += cost;
        k_out++;
    }
    return head;
}

static Node* get_top_k_single_word(int k, const string& word, QNA_tool& q) {
    Tries<pair<int, pair<int, int>>, int>* trie = q.trie->get(word, 0);
    if (!trie) return nullptr;
    vector<pair<pair<int, pair<int, int>>, int>> entries;
    trie->data.get_all(entries);
    if (entries.empty()) return nullptr;
    Heap<pair<int, pair<int, pair<int, int>>>> heap;
    for (auto& entry : entries) {
        if (heap.get_size() < static_cast<size_t>(k)) {
            heap.insert({entry.second, entry.first});
        } else if (heap.get_top().first < entry.second) {
            heap.pop();
            heap.insert({entry.second, entry.first});
        }
    }
    Node* head = nullptr;
    while (heap.get_size()) {
        Node* node = new Node();
        node->book_code = heap.get_top().second.first;
        node->page = heap.get_top().second.second.first;
        node->paragraph = heap.get_top().second.second.second;
        node->right = head;
        if (head) head->left = node;
        head = node;
        heap.pop();
    }
    return head;
}

static pair<Node*, int> get_analysis(string query, QNA_tool& q) {
    vector<pair<string, int>> words = rake(query);
    int per_word = words.empty() ? 400 : 400 / (words.size() + 1);
    Graph graph;
    for (auto& item : words) {
        Node* list = get_top_k_single_word(per_word, item.first, q);
        int taken = 0;
        while (list && taken < per_word) {
            int total_words = q.counter->get({list->book_code, {list->page, list->paragraph}});
            if (total_words > 15) {
                graph.add_node(list->book_code, list->page, list->paragraph, item, total_words);
                taken++;
            }
            list = list->right;
        }
    }
    vector<pair<pair<int, pair<int, int>>, double>> scores = graph.get_score();
    if (!scores.empty()) merge_scores(scores, 0, scores.size() - 1);
    int k = 0;
    Node* head = gather_top(scores, q, k);
    return {head, k};
}

QNA_tool::QNA_tool() {
    trie = new Tries<pair<int, pair<int, int>>, int>();
    counter = new AVLMap<pair<int, pair<int, int>>, int>();
    extract_csv();
}

QNA_tool::~QNA_tool() {
    delete trie;
    delete counter;
}

static inline bool is_separator(char c) {
    static const string separators = " .,-:!\"'()?—[]“”‘’˙;@";
    return separators.find(c) != string::npos;
}

void QNA_tool::insert_sentence(int book_code, int page, int paragraph, int sentence_no, string sentence) {
    string token;
    int count = 0;
    for (char ch : sentence) {
        if (is_separator(ch)) {
            if (!token.empty()) {
                trie->increase_by_1(token, {book_code, {page, paragraph}}, 0);
                count++;
                token.clear();
            }
        } else {
            token.push_back(ch >= 'A' && ch <= 'Z' ? static_cast<char>(ch - 'A' + 'a') : ch);
        }
    }
    if (!token.empty()) {
        trie->increase_by_1(token, {book_code, {page, paragraph}}, 0);
        count++;
    }
    counter->increase_by_x({book_code, {page, paragraph}}, count);
}

void QNA_tool::update_avl(AVLMap<pair<int, pair<int, int>>, double>& scores, vector<pair<pair<int, pair<int, int>>, int>>& vals, double total) {
    for (auto& entry : vals) {
        scores.increase_by_x(entry.first, entry.second * total);
    }
}

Node* QNA_tool::get_top_k_para(string query, int k) {
    AVLMap<pair<int, pair<int, int>>, double> scores;
    string word;
    string separators = " .,-:!\"'()?—[]“”‘’˙;@";
    for (char ch : query) {
        if (separators.find(ch) != string::npos) {
            if (!word.empty()) {
                Tries<pair<int, pair<int, int>>, int>* node = trie->get(word, 0);
                if (node) {
                    vector<pair<pair<int, pair<int, int>>, int>> entries;
                    node->data.get_all(entries);
                    update_avl(scores, entries, (node->total + 1.0) / (node->c_val + 1.0));
                }
                word.clear();
            }
        } else {
            word.push_back(ch >= 'A' && ch <= 'Z' ? static_cast<char>(ch - 'A' + 'a') : ch);
        }
    }
    if (!word.empty()) {
        Tries<pair<int, pair<int, int>>, int>* node = trie->get(word, 0);
        if (node) {
            vector<pair<pair<int, pair<int, int>>, int>> entries;
            node->data.get_all(entries);
            update_avl(scores, entries, (node->total + 1.0) / (node->c_val + 1.0));
        }
    }
    vector<pair<pair<int, pair<int, int>>, double>> items;
    scores.get_all(items);
    Heap<pair<double, pair<int, pair<int, int>>>> heap;
    for (auto& entry : items) {
        if (heap.get_size() < static_cast<size_t>(k)) {
            heap.insert({entry.second, entry.first});
        } else if (heap.get_top().first < entry.second) {
            heap.pop();
            heap.insert({entry.second, entry.first});
        }
    }
    Node* head = nullptr;
    while (heap.get_size()) {
        Node* node = new Node();
        node->book_code = heap.get_top().second.first;
        node->page = heap.get_top().second.second.first;
        node->paragraph = heap.get_top().second.second.second;
        node->right = head;
        if (head) head->left = node;
        head = node;
        heap.pop();
    }
    return head;
}

void QNA_tool::query(string question, string filename) {
    pair<Node*, int> analysis = get_analysis(question, *this);
    const char* api_key = std::getenv("OPENAI_API_KEY");
    if (!api_key) {
        std::cerr << "Error: OPENAI_API_KEY environment variable is not set." << std::endl;
        return;
    }
    query_llm(filename, analysis.first, analysis.second, api_key, question);
}

std::string QNA_tool::get_paragraph(int book_code, int page, int paragraph) {
    std::cout << "Book_code: " << book_code << " Page: " << page << " Paragraph: " << paragraph << std::endl;
    std::string filename = "corpus/mahatma-gandhi-collected-works-volume-" + std::to_string(book_code) + ".txt";
    std::ifstream inputFile(filename);
    std::string tuple;
    std::string sentence;
    if (!inputFile.is_open()) {
        std::cerr << "Error: Unable to open the input file " << filename << "." << std::endl;
        exit(1);
    }
    std::string res;
    while (std::getline(inputFile, tuple, ')') && std::getline(inputFile, sentence)) {
        tuple += ')';
        int metadata[5];
        std::istringstream iss(tuple);
        std::string token;
        iss.ignore(1);
        int idx = 0;
        while (std::getline(iss, token, ',')) {
            size_t start = token.find_first_not_of(" ");
            size_t end = token.find_last_not_of(" ");
            if (start != std::string::npos && end != std::string::npos) token = token.substr(start, end - start + 1);
            if (token[0] == '\'') {
                metadata[idx++] = std::stoi(token.substr(1, token.length() - 2));
            } else {
                metadata[idx++] = std::stoi(token);
            }
        }
        if (metadata[0] == book_code && metadata[1] == page && metadata[2] == paragraph) {
            res += sentence;
        }
    }
    inputFile.close();
    return res;
}

void QNA_tool::extract_csv() {
    ifstream file("unigram_freq.csv");
    string line;
    getline(file, line);
    while (getline(file, line)) {
        size_t pos = line.find(',');
        if (pos == string::npos) continue;
        string word = line.substr(0, pos);
        long long number = 0;
        for (size_t i = pos + 1; i < line.size(); ++i) {
            number = number * 10 + (line[i] - '0');
        }
        trie->insert(word, number, 0);
    }
}

void QNA_tool::query_llm(string filename, Node* root, int k, string API_KEY, string question) {
    Node* cur = root;
    int count = 0;
    while (count < k) {
        assert(cur != nullptr);
        string fname = "paragraph_" + std::to_string(count) + ".txt";
        remove(fname.c_str());
        ofstream out(fname);
        string paragraph = get_paragraph(cur->book_code, cur->page, cur->paragraph);
        assert(paragraph != "$I$N$V$A$L$I$D$");
        out << paragraph;
        cur = cur->right;
        count++;
    }
    ofstream query_file("query.txt");
    query_file << "These are the excerpts from Mahatma Gandhi's books.\nOn the basis of this, " << question;
    query_file.close();
    string command = "python3 ";
    command += filename;
    command += " ";
    command += API_KEY;
    command += " ";
    command += std::to_string(k);
    command += " query.txt";
    system(command.c_str());
}
