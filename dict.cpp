#include "dict.h"

namespace {

inline char lower_char(char c) {
    return (c >= 'A' && c <= 'Z') ? static_cast<char>(c - 'A' + 'a') : c;
}

string normalize(const string& word) {
    string out;
    out.reserve(word.size());
    for (char c : word) out.push_back(lower_char(c));
    return out;
}

inline bool is_delim(char c) {
    static const string separators = " .,-:!\"'()?—[]“”‘’˙;@";
    return separators.find(c) != string::npos;
}

}

TrieNode::TrieNode() : children(new AVLTree()), word(""), pres_word(""), word_count(0), par(nullptr) {}

TrieNode::TrieNode(string word) : children(new AVLTree()), word(normalize(word)), pres_word(""), word_count(0), par(nullptr) {}

TrieNode::TrieNode(string word, string past)
    : children(new AVLTree()), word(normalize(word)), pres_word(past), word_count(0), par(nullptr) {}

TrieNode* TrieNode::get_child(char c) {
    return children->find(lower_char(c));
}

void TrieNode::set_child(char c, TrieNode* node) {
    children->insert(node, lower_char(c));
}

int AVLTree::height(AVLnode* node) {
    return node ? node->height : -1;
}

AVLnode* AVLTree::rotate_left(AVLnode* node) {
    AVLnode* pivot = node->left;
    node->left = pivot->right;
    pivot->right = node;
    node->height = max(height(node->left), height(node->right)) + 1;
    pivot->height = max(height(pivot->left), height(pivot->right)) + 1;
    return pivot;
}

AVLnode* AVLTree::rotate_right(AVLnode* node) {
    AVLnode* pivot = node->right;
    node->right = pivot->left;
    pivot->left = node;
    node->height = max(height(node->left), height(node->right)) + 1;
    pivot->height = max(height(pivot->left), height(pivot->right)) + 1;
    return pivot;
}

AVLnode* AVLTree::recur(TrieNode* child, char ch, AVLnode* node) {
    ch = lower_char(ch);
    if (!node) return new AVLnode(ch, child);
    if (node->c > ch) {
        node->left = recur(child, ch, node->left);
    } else if (node->c < ch) {
        node->right = recur(child, ch, node->right);
    } else {
        node->next = child;
        return node;
    }
    int lh = height(node->left);
    int rh = height(node->right);
    node->height = max(lh, rh) + 1;
    if (abs(lh - rh) <= 1) return node;
    if (lh > rh) {
        if (height(node->left->left) < height(node->left->right)) node->left = rotate_right(node->left);
        return rotate_left(node);
    } else {
        if (height(node->right->right) < height(node->right->left)) node->right = rotate_left(node->right);
        return rotate_right(node);
    }
}

void AVLTree::insert(TrieNode* node, char ch) {
    root = recur(node, ch, root);
}

TrieNode* AVLTree::find(char ch) {
    AVLnode* cur = root;
    ch = lower_char(ch);
    while (cur) {
        if (cur->c == ch) return cur->next;
        cur = (cur->c > ch) ? cur->left : cur->right;
    }
    return nullptr;
}

void AVLTree::addALL(vector<TrieNode*>& stack, AVLnode* node) {
    if (!node) return;
    if (node->next) stack.push_back(node->next);
    addALL(stack, node->left);
    addALL(stack, node->right);
}

void AVLTree::addALL(vector<TrieNode*>& stack) {
    addALL(stack, root);
}

Trie::Trie() : root(new TrieNode()) {}

void Trie::insert(string word) {
    if (word.empty()) return;
    TrieNode* child = root->get_child(word[0]);
    if (!child) {
        TrieNode* fresh = new TrieNode(word, word);
        fresh->par = root;
        root->set_child(word[0], fresh);
        fresh->word_count++;
        return;
    }
    TrieNode* node = child;
    size_t idx = 0;
    const size_t limit = word.size();
    while (idx < limit) {
        size_t matched = 0;
        while (idx + matched < limit && matched < node->word.size() && word[idx + matched] == node->word[matched]) {
            matched++;
        }
        if (matched < node->word.size()) {
            char branch = node->word[0];
            TrieNode* split = new TrieNode(node->word.substr(0, matched), node->par->pres_word + node->word.substr(0, matched));
            split->par = node->par;
            split->par->set_child(branch, split);
            node->word = node->word.substr(matched);
            split->set_child(node->word[0], node);
            node->par = split;
            node = split;
        }
        idx += matched;
        if (idx == limit) break;
        if (!node->get_child(word[idx])) {
            TrieNode* fresh = new TrieNode(word.substr(idx), word);
            fresh->par = node;
            node->set_child(word[idx], fresh);
            node = fresh;
            break;
        }
        node = node->get_child(word[idx]);
    }
    node->word_count++;
}

int Trie::get_count(string word) {
    TrieNode* node = root;
    size_t idx = 0;
    while (node && idx < word.size()) {
        for (size_t j = 0; j < node->word.size(); ++j, ++idx) {
            if (idx == word.size() || word[idx] != node->word[j]) return 0;
        }
        if (idx == word.size()) break;
        node = node->get_child(word[idx]);
    }
    return node ? node->word_count : 0;
}

void Trie::write_to_file(string filename) {
    fstream file(filename, ios::out);
    if (!file.is_open()) return;
    vector<TrieNode*> pending;
    pending.push_back(root);
    while (!pending.empty()) {
        TrieNode* cur = pending.back();
        pending.pop_back();
        if (cur->word_count) file << cur->pres_word << ", " << cur->word_count << endl;
        cur->children->addALL(pending);
    }
}

Trie::~Trie() {
    delete root;
}

Dict::Dict() : t(new Trie()) {}

Dict::~Dict() {
    delete t;
}

void Dict::insert_sentence(int book_code, int page, int paragraph, int sentence_no, string sentence) {
    string token;
    token.reserve(sentence.size());
    for (char c : sentence) {
        if (is_delim(c)) {
            if (!token.empty()) {
                t->insert(token);
                token.clear();
            }
        } else {
            token.push_back(lower_char(c));
        }
    }
    if (!token.empty()) t->insert(token);
}

int Dict::get_word_count(string word) {
    return t->get_count(normalize(word));
}

void Dict::dump_dictionary(string filename) {
    t->write_to_file(filename);
}
