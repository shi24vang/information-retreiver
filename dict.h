// Do NOT add any other includes
#include <string> 
#include <vector>
#include <iostream>
#include <fstream>
using namespace std;
//declaration
struct TrieNode;
struct AVLnode;
class AVLTree;
class Trie;
//TrieNode definition
struct TrieNode{
	AVLTree* children;
	string word,pres_word;//pres_word stores word from root
	int word_count;
	TrieNode* par=nullptr;
	TrieNode();
	TrieNode(string word);
	TrieNode(string word,string past);
    ~TrieNode(){}//TODO:check memory leaks
	TrieNode* get_child(char c);
	void set_child(char c,TrieNode* i);
};

//AVLnode tree definition
struct AVLnode{
    TrieNode* next;
    char c;
    AVLnode* left=nullptr;
    AVLnode* right=nullptr;
    int height=0;
    AVLnode(char c,TrieNode* next):next(next),c(c){}
    ~AVLnode(){if(left)delete left;if(right)delete right;delete next;}
};

//AVL tree declaration
class AVLTree{
private:
    AVLnode* root;
    int height(AVLnode* i);
    AVLnode* recur(TrieNode* i,char c,AVLnode* r);
    AVLnode* rotate_left(AVLnode* i);
    AVLnode* rotate_right(AVLnode* i);
    void addALL(vector<TrieNode*>& st,AVLnode* i);
public:
    AVLTree():root(nullptr){}
    void insert(TrieNode* i,char c);
    TrieNode* find(char c);
    ~AVLTree(){if(root)delete root;}
    void addALL(vector<TrieNode*>& st);
};

//Trie definition
class Trie{
private:
	TrieNode* root;
public:
	Trie();
	void insert(string word);
	int get_count(string word);
	void write_to_file(string filename);
	~Trie();
};
class Dict {
private:
    // You can add attributes/helper functions here
    Trie* t;
public: 
    /* Please do not touch the attributes and 
    functions within the guard lines placed below  */
    /* ------------------------------------------- */
    Dict();

    ~Dict();

    void insert_sentence(int book_code, int page, int paragraph, int sentence_no, string sentence);

    int get_word_count(string word);

    void dump_dictionary(string filename);

    /* -----------------------------------------*/
};