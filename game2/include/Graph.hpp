#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <string>
#include <array>
#include <vector>
using namespace std;

struct Node {
    bool end = false;
    array<Node*, 26> nodeCon;

    Node();
    ~Node();
};

class Graph {
private:
    Node* root;  
    static Graph* instance;  

    Graph(); 

public:
    static Graph* getInstance(); 

    ~Graph();  

    void insertWord(const string& word);  
    bool isWordInDictionary(const string& word); 
};


void loadDictionary(const string& fileName, Graph& gDictionary);

#endif  