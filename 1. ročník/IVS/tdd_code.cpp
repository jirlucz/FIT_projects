//======== Copyright (c) 2023, FIT VUT Brno, All rights reserved. ============//
//
// Purpose:     Test Driven Development - graph
//
// $NoKeywords: $ivs_project_1 $tdd_code.cpp
// $Author:     Jiri Kotek <xkotek09@stud.fit.vutbr.cz>
// $Date:       $2023-03-03
//============================================================================//
/**
 * @file tdd_code.cpp
 * @author Martin Dočekal
 * @author Karel Ondřej
 *
 * @brief Implementace metod tridy reprezentujici graf.
 */

#include "tdd_code.h"


Graph::Graph(){}

Graph::~Graph(){}

std::vector<Node*> Graph::nodes() {
    std::vector<Node*> nodes = nodeList;
    
    return nodes;
}

std::vector<Edge> Graph::edges() const{
    std::vector<Edge> edges = edgeList;

    return edges;
}

Node* Graph::addNode(size_t nodeId) {

    Node* newnode = new Node();
    newnode->id = nodeId;
    newnode->color = 0;

    for (
        int i = 0; i < nodeList.size(); i++)
    {
        if (nodeId == nodeList[i]->id)
        {
            return nullptr;
        }
    }

    nodeList.push_back(newnode);

    return newnode;
}

bool Graph::addEdge(const Edge& edge){
    
    bool contain = false;


    if (edge.a == edge.b)
    {
        return false;
    }

    for (auto newedge : edgeList)
    {
        if (newedge == edge)
        {
            contain = true;
            return false;
        }
    }
    
    if (contain != true)
    {
        addNode(edge.a);
        addNode(edge.b);
        edgeList.push_back(edge);
        return true;
    }

    else
        return false;

}

void Graph::addMultipleEdges(const std::vector<Edge>& edges) {
     for (const auto& edge : edges) {
        addEdge(edge);
    }
}

Node* Graph::getNode(size_t nodeId){

     for (int i = 0; i < nodeList.size(); i++)
    {
        if (nodeId == nodeList[i]->id)
        {
            return nodeList[i];
        }
    }

    return nullptr;
}

bool Graph::containsEdge(const Edge& edge) const{

    Edge FindEdge = edge;
    bool Find = false;

    for (auto FindEdge : edgeList)
    {
        if (FindEdge == edge)
        {
            Find = true;
            break;
        }
    }

    return Find;
}

void Graph::removeNode(size_t nodeId){

    bool Removed = false;

    //pruchod polem nodeList, pokud node nenalezne, tak vzhodi vyhodi vyjimku
    for (int i = 0; i < nodeList.size(); i++)
    {
        if (nodeId == nodeList[i]->id)
        {
            for (int j = edgeList.size() - 1; j >= 0; j--)
            {
                if (nodeId == edgeList[j].a || nodeId == edgeList[j].b)
                {
                    edgeList.erase(edgeList.begin() + j);
                }
            }

            nodeList.erase(nodeList.begin() + i);
            Removed = true;
            break;
        }
    }

    if (Removed != true)
    {
        throw std::out_of_range("Node ID not found in graph");
    }

}

void Graph::removeEdge(const Edge& edge){

    bool FindEdge = false;

    for (int i = 0; i < edgeList.size(); i++)
    {
        Edge edgeInLins = edgeList.at(i);

         if (edge == edgeInLins)
        {
            edgeList.erase(edgeList.begin() + i);
            FindEdge = true;
        }
    }
    
    if (FindEdge != true)
    {
        throw std::out_of_range("Edge not found in graph");
    }
    

}

size_t Graph::nodeCount() const{

    return nodeList.size() ;
}

size_t Graph::edgeCount() const{
    return edgeList.size();
}

size_t Graph::nodeDegree(size_t nodeId) const{

    bool Contain = false;
    int NodeIdDegree = 0;

    //Kontorola, jestli se bod v grafu nachazi
    for (int i = 0; i < nodeList.size(); i++)
    {
        if (nodeId == nodeList[i]->id)
        {
           Contain = true;
        }
    }

    //pokud ne, vyhod vyjimku
    if (Contain != true)
    {
        throw std::out_of_range("Node ID not found in graph");
    }
    //konec kontroly seznamu

    //Pruchod polem Edge a potovnani, jestli obsahuje Node
    for (int i = 0; i < edgeList.size(); i++)
    {
        if (nodeId == edgeList[i].a || nodeId == edgeList[i].b)
        {
            NodeIdDegree++;
        }     
    }
    return NodeIdDegree;
}

size_t Graph::graphDegree() const{
    int MaxDegree = 0;

    for (int i = 0; i < nodeList.size(); i++)
    {
        int degree = nodeDegree(nodeList[i]->id);
        if (degree > MaxDegree) {
            MaxDegree = degree;
        }
    }
    
    return MaxDegree;
}

void Graph::coloring(){

    //nastaveni vsech barev uzlu na 1
    for (auto node : nodeList){
        node->color = 1;
    }

    int actualColor = 1;

    //prochazeni vektoru
    for (auto edge : edgeList){
        actualColor += actualColor;

        Node* p_IDa = getNode(edge.a);
        Node* p_IDb = getNode(edge.b);

        actualColor += actualColor;

        if (p_IDa->color ==  p_IDb->color)
        {
            p_IDb->color = actualColor+1;
        }
        
    }    
}

void Graph::clear() {


    for (int i = edgeList.size() - 1; i >= 0; i--)
    {
        edgeList.erase(edgeList.begin() + i);
        
    }

    for (int j = nodeList.size() - 1; j >= 0; j--)
    {
        nodeList.erase(nodeList.begin() + j);
    }
}

/*** Konec souboru tdd_code.cpp ***/
