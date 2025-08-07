#include "Graph.hpp"
#include <iostream>
#include <fstream>
#include <cctype>

Graph* Graph::instance = nullptr;

Node::Node() {
    nodeCon.fill(nullptr);
}

Node::~Node() {
    for (auto c : nodeCon) {
        delete c;
    }
}

Graph::Graph() {
    root = new Node();
}

Graph::~Graph() {
    delete root;
}

Graph* Graph::getInstance() {
    if (instance == nullptr) {
        instance = new Graph();
    }
    return instance;
}

void Graph::insertWord(const string& word) {
    Node* current = root;
    for (auto c : word) {
        int viTri = c - 'a';
        if (current->nodeCon[viTri] == nullptr) {
            current->nodeCon[viTri] = new Node();
        }
        current = current->nodeCon[viTri];
    }
    current->end = true;
}

bool Graph::isWordInDictionary(const string& word) {
    Node* current = root;
    for (auto c : word) {
        c = tolower(c);
        int viTri = c - 'a';
        if (current->nodeCon[viTri] == nullptr) {
            return false;
        }
        current = current->nodeCon[viTri];
    }
    return current->end;
}

void loadDictionary(const string& fileName, Graph& gDictionary) {
    ifstream file(fileName);
    if (!file.is_open()) {
        cerr << "Chưa mở được file từ điển" << endl;
        exit(1);
    }
    string word;
    while (getline(file, word)) {
        gDictionary.insertWord(word);
    }
}
