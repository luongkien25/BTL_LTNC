#include <iostream>
#include <fstream>
#include <string>
#include <array>
using namespace std;
// dùng đồ thị cây lưu trữ các từ
struct Node {
    bool end = false;
    array<Node*,26> nodeCon = {};

    Node() {
        nodeCon.fill(nullptr);
    }
    ~Node() {
        for(auto c : nodeCon) {
            delete c; 
        }
    }
};

class Graph {
private:    
    Node* root;  
    static Graph* instance;  //đảm bảo có duy nhất 1 graph;

    Graph() {
        root = new Node();
    }
public: 
    static Graph* getInstance() {
        if (instance == nullptr) {
            instance = new Graph();  
        }
        return instance;  
    }
    ~Graph() {
        delete root;
    }
    void insertWord(const string& word) {
        Node* current = root;
        for (auto c : word) {
            int viTri = c - 'a'; // vị trí của node con.
            if(current->nodeCon[viTri] == nullptr) {
                current->nodeCon[viTri] = new Node();
            }
            current = current->nodeCon[viTri];
        }
        current->end = true;
    }

    bool isWordInDictionary(const string& word) {
        Node* current = root;
        for (auto c : word) {
            int viTri = c - 'a';
            if(current->nodeCon[viTri] == nullptr) {
                return false;
            }
            current = current->nodeCon[viTri];
        }
        return current->end;
    }
};

Graph* Graph::instance = nullptr;

void loadDictionary (const string& fileName, Graph& gDictionary) {
    ifstream file(fileName);
    if (!file.is_open()) {
        cerr << "Chưa mở được file từ điển" << endl;
        exit(1);
    } 
    string word;
    while(getline(file, word)) {
        gDictionary.insertWord(word);
    }
}

int main() { // test thử
    Graph* gDictionary = Graph::getInstance();
    loadDictionary("EnglishDictionary.txt", *gDictionary);
    string word;
    cin >> word;
    if(!gDictionary->isWordInDictionary(word)) {
        cout << word << " is not a word";
    } else {
        cout << word << " is a word";
    }
    return 0;
}

