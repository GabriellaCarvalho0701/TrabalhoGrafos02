#ifndef EDGE_H
#define EDGE_H

#include <iostream>

#include "../Graph/Graph.h"
#include "../Node/node.h"

class Graph;
class Node;
class Edge {
    public: 
        Edge(Node *headNode, Node *tailNode, float weight, Graph *graph);
        ~Edge() = default;

        void setNextEdge(Edge* nextEdge);
        Edge* getNextEdge();
       
        int getId();
        Node* getTailNode();
        Node* getHeadNode();

        int getWeight();
        int edgeCost(Node *nodeHead, Node *tailNode);
        
    private:
        int id;
        float weight;
        Node *tailNode;
        Node *headNode;
        Edge *nextEdge;   
};

#endif