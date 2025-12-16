// Do NOT add any other includes
#include "search.h"

namespace {

inline char norm(char c) {
    return (c >= 'A' && c <= 'Z') ? static_cast<char>(c - 'A' + 'a') : c;
}

}

SearchEngine::SearchEngine() {}

SearchEngine::~SearchEngine() {}

long long int SearchEngine::hash(string s) {
    long long int value = 0;
    for (char c : s) {
        value = (value * seed + norm(c)) % mod;
    }
    return value;
}

long long int SearchEngine::power(int a, int b) {
    if (!b) return 1;
    long long int half = power(a, b / 2);
    long long int result = (half * half) % mod;
    if (b & 1) result = (result * a) % mod;
    return result;
}

void SearchEngine::insert_sentence(int book_code, int page, int paragraph, int sentence_no, string sentence) {
    this->sentence.push_back(sentence);
    this->book_code.push_back(book_code);
    this->page.push_back(page);
    this->paragraph.push_back(paragraph);
    this->sentence_no.push_back(sentence_no);
    this->position.push_back(static_cast<int>(sentence.size()));
}

char SearchEngine::conv(char a) {
    return norm(a);
}

Node* SearchEngine::search(string pattern, int& n_matches) {
    n_matches = 0;
    if (pattern.empty()) return nullptr;
    int len = pattern.size();
    long long int pattern_hash = hash(pattern);
    Node* head = nullptr;
    for (int idx = static_cast<int>(sentence.size()) - 1; idx >= 0; --idx) {
        if (sentence[idx].size() < static_cast<size_t>(len)) continue;
        long long int window_hash = hash(sentence[idx].substr(0, len));
        for (int pos = len - 1; pos <= static_cast<int>(sentence[idx].size()); ++pos) {
            if (window_hash == pattern_hash) {
                bool ok = true;
                for (int k = 0; k < len; ++k) {
                    if (conv(sentence[idx][pos - len + 1 + k]) != conv(pattern[k])) {
                        ok = false;
                        break;
                    }
                }
                if (ok) {
                    n_matches++;
                    Node* node = new Node(book_code[idx], page[idx], paragraph[idx], sentence_no[idx], pos - len + 1);
                    node->left = nullptr;
                    node->right = head;
                    if (head) head->left = node;
                    head = node;
                }
            }
            if (pos == static_cast<int>(sentence[idx].size())) break;
            window_hash = (window_hash * seed + conv(sentence[idx][pos + 1]) -
                           power(seed, len) * conv(sentence[idx][pos - len + 1])) %
                          mod;
            if (window_hash < 0) window_hash += mod;
        }
    }
    return head;
}
