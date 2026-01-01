// Do NOT add any other includes
#include <string> 
#include <vector>
#include <iostream>
#include "Node.h"
using namespace std;
class SearchEngine {
private:
    // You can add attributes/helper functions here
	vector<string> sentence;
	vector<int> book_code,page,paragraph,sentence_no,position;
	int seed=131;
	int mod=1000000007;
	long long int hash(string s);
	long long int power(int a,int b);
    char conv(char a);
public: 
    /* Please do not touch the attributes and 
    functions within the guard lines placed below  */
    /* ------------------------------------------- */
    SearchEngine();

    ~SearchEngine();

    void insert_sentence(int book_code, int page, int paragraph, int sentence_no, string sentence);

    Node* search(string pattern, int& n_matches);

    /* -----------------------------------------*/
};
