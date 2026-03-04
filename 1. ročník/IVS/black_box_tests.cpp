//======== Copyright (c) 2017, FIT VUT Brno, All rights reserved. ============//
//
// Purpose:     Red-Black Tree - public interface tests
//
// $NoKeywords: $ivs_project_1 $black_box_tests.cpp
// $Author:     Jiri Kotek <xkotek09@stud.fit.vutbr.cz>
// $Date:       $2023-02-28
//============================================================================//
/**
 * @file black_box_tests.cpp
 * @author Jiri Kotek
 * 
 * @brief Implementace testu binarniho stromu.
 */

#include <vector>

#include "gtest/gtest.h"

#include "red_black_tree.h"

//============================================================================//
// ** ZDE DOPLNTE TESTY **
//
// Zde doplnte testy Red-Black Tree, testujte nasledujici:
// 1. Verejne rozhrani stromu
//    - InsertNode/DeleteNode a FindNode
//    - Chovani techto metod testuje pro prazdny i neprazdny strom.
// 2. Axiomy (tedy vzdy platne vlastnosti) Red-Black Tree:
//    - Vsechny listove uzly stromu jsou *VZDY* cerne.
//    - Kazdy cerveny uzel muze mit *POUZE* cerne potomky.
//    - Vsechny cesty od kazdeho listoveho uzlu ke koreni stromu obsahuji
//      *STEJNY* pocet cernych uzlu.
//============================================================================//

class EmptyTree : public :: testing :: Test{

protected:
    std::vector<BinaryTree::Node_t*> allNodes;
    BinaryTree tree;
};


class NonEmptyTree : public :: testing :: Test{

protected: 
    virtual void SetUp (){
        int values [] = {5,1,22,15,10,2};

        for (auto value : values )
        {
            tree.InsertNode(value);
        }
    }
    BinaryTree tree;
};


class TreeAxioms : public :: testing :: Test{

protected:
    std::vector<BinaryTree::Node_t*> allNodes;
    std::vector<BinaryTree::Node_t*> allLeafNodes;
    std::vector<BinaryTree::Node_t*> allNonLeafNodes;
    virtual void SetUp (){
        int values [] = {5,1,22,15,10,2};

        for (auto value : values )
        {
            tree.InsertNode(value);
        }
    }
    BinaryTree tree;
};




TEST_F(EmptyTree, InsertNode_single){
    tree.GetAllNodes(allNodes);
    EXPECT_TRUE(allNodes.empty());

        auto Node = tree.InsertNode(8);
    EXPECT_NE(tree.FindNode(8), nullptr);
    EXPECT_EQ(Node.first, true);
    EXPECT_EQ(Node.second->key , 8);

        auto Node2 = tree.InsertNode(8);
    EXPECT_EQ(Node2.first , false);

    tree.GetAllNodes(allNodes);
    EXPECT_EQ (allNodes[0]->key ,8); 

    EXPECT_EQ (Node.second->color, BLACK );
    EXPECT_EQ (Node.second->pParent, nullptr );

}

TEST_F(EmptyTree, InsertNode_multiple){
    tree.GetAllNodes(allNodes);
    EXPECT_TRUE(allNodes.empty());

    auto Node = tree.InsertNode(1);

    auto Node2 = tree.InsertNode(2);

    auto Node3 = tree.InsertNode(5);

    EXPECT_EQ(Node.second->key , 1);
    EXPECT_EQ(Node2.second->key , 2);
    EXPECT_EQ(Node3.second->key , 5);

    EXPECT_EQ(Node.second->color , RED);
    EXPECT_EQ(Node2.second->color , BLACK);
    EXPECT_EQ(Node3.second->color , RED);
}

TEST_F(EmptyTree, DeleteNode){
    EXPECT_EQ(tree.DeleteNode(10), false);
    EXPECT_EQ(tree.DeleteNode(0), false);
    EXPECT_EQ(tree.DeleteNode(-5), false);
}


TEST_F(EmptyTree, FindNode){
   EXPECT_EQ(tree.FindNode(100), nullptr);
   EXPECT_EQ(tree.FindNode(0), nullptr);
   EXPECT_EQ(tree.FindNode(-15), nullptr);
}
///////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////

TEST_F(NonEmptyTree, InsertNode){
        auto Node = tree.InsertNode(1);
    EXPECT_EQ(Node.first, false);
    EXPECT_EQ(Node.second->key, 1);

    ASSERT_EQ(tree.FindNode(3), nullptr);
        auto Node3 = tree.InsertNode(3);
    ASSERT_NE(tree.FindNode(3), nullptr);
    EXPECT_EQ(Node3.first, true);
    EXPECT_EQ(Node3.second->key, 3);
    EXPECT_EQ(Node3.second->color, RED);
     
}

TEST_F(NonEmptyTree, DeleteNode){
    ASSERT_NE(tree.FindNode(22), nullptr);

    auto Node22 = tree.DeleteNode(22);;

    EXPECT_EQ( Node22, true );
    EXPECT_EQ(tree.FindNode(22), nullptr);

       auto nNode22 = tree.InsertNode(22);
    EXPECT_EQ (nNode22.first, true);
    EXPECT_EQ (nNode22.second->key, 22);
}

TEST_F(NonEmptyTree, FindNode){
    auto node = tree.FindNode(15);
    ASSERT_NE(node, nullptr);

    EXPECT_EQ(node->key, 15 );

    EXPECT_EQ(tree.FindNode(0), nullptr);
}

TEST_F(TreeAxioms, Axiom1) {
    tree.GetLeafNodes(allLeafNodes);

    for (auto LeafNode : allLeafNodes)
    {
        EXPECT_EQ (LeafNode->color, BLACK); 
    }
}

TEST_F(TreeAxioms, Axiom2){
    tree.GetAllNodes(allNodes);

    for (auto node : allNodes)
    {
        if (node->color == RED)
        {
            EXPECT_EQ(node->pLeft->color, BLACK);
            EXPECT_EQ(node->pRight->color, BLACK);
        }
    }
}

TEST_F(TreeAxioms, Axiom3){
    int counter = 0;
    tree.GetLeafNodes(allLeafNodes);

    for (auto LeafNode :  allLeafNodes)
    {
        while (LeafNode != nullptr && LeafNode->pParent != nullptr  )
        {
            if (LeafNode->pParent->color == BLACK)
            {
                counter++;
            }
            
            LeafNode = LeafNode->pParent;
        }
        break;
    }

    for (auto LeafNode :  allLeafNodes)
    {
        int ControlCounter = 0;
        while (LeafNode != nullptr && LeafNode->pParent != nullptr  )
        {
            
            if (LeafNode->pParent->color == BLACK)
            {
                ControlCounter++;
            }
            
            LeafNode = LeafNode->pParent;
        }
        EXPECT_EQ (ControlCounter, counter);
    }
}





/*** Konec souboru black_box_tests.cpp ***/
