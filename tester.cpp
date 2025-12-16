#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "Node.h"
#include "qna_tool.h"

using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    QNA_tool qna_tool;
    const int num_books = 98;

    for (int book = 1; book <= num_books; ++book) {
        cout << "Inserting book " << book << endl;
        string filename = "corpus/mahatma-gandhi-collected-works-volume-" + to_string(book) + ".txt";
        ifstream input(filename);
        if (!input.is_open()) {
            cerr << "Error: Unable to open the input file mahatma-gandhi." << endl;
            continue;
        }
        string tuple;
        string sentence;
        while (getline(input, tuple, ')') && getline(input, sentence)) {
            tuple.push_back(')');
            vector<int> metadata;
            metadata.reserve(4);
            istringstream iss(tuple);
            string token;
            iss.ignore(1);
            while (getline(iss, token, ',')) {
                size_t start = token.find_first_not_of(' ');
                size_t end = token.find_last_not_of(' ');
                if (start != string::npos && end != string::npos) token = token.substr(start, end - start + 1);
                if (!token.empty() && token[0] == '\'') {
                    metadata.push_back(stoi(token.substr(1, token.size() - 2)));
                } else {
                    metadata.push_back(stoi(token));
                }
            }
            qna_tool.insert_sentence(metadata[0], metadata[1], metadata[2], metadata[3], sentence);
        }
    }

    string question = "What is the date of birth of Mahatma Gandhi?";
    Node* head = qna_tool.get_top_k_para(question, 5);
    vector<string> paragraphs;
    while (head) {
        paragraphs.push_back(qna_tool.get_paragraph(head->book_code, head->page, head->paragraph));
        head = head->right;
    }
    for (const auto& para : paragraphs) {
        cout << para << "\n\n\n";
    }
     qna_tool.query(question, "api_call.py");
    return 0;
}
