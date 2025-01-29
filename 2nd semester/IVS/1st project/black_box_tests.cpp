//======== Copyright (c) 2017, FIT VUT Brno, All rights reserved. ============//
//
// Purpose:     Red-Black Tree - public interface tests
//
// $NoKeywords: $ivs_project_1 $black_box_tests.cpp
// $Author:     Pavel Heřmann <xherma34@stud.fit.vutbr.cz>
// $Date:       $2021-03-03
//============================================================================//
/**
 * @file black_box_tests.cpp
 * @author Pavel Heřmann 
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

class NonEmptyTree : public ::testing::Test
{
protected:
    virtual void SetUp() {
        int values[] = { 50, 85, 15, 70, 19, 60, 30, 10, 65, 80, 90, 40, 5, 55 };

        for(int value : values)
        {
            tree.InsertNode(value);
        }
    }

    BinaryTree tree;
};

class EmptyTree : public ::testing::Test
{
protected:
    BinaryTree tree;
};

class TreeAxioms : public ::testing::Test
{
protected:
    virtual void SetUp() {
        int values[] = { 50, 85, 15, 70, 19, 60, 30, 10, 65, 80, 90, 40, 5, 55 };

        for(int value : values)
        {
            tree.InsertNode(value);
        }
    }

    BinaryTree tree;
};

TEST_F(EmptyTree, InsertNode)
{

    std::pair<bool, BinaryTree::Node_t *> singleNode = tree.InsertNode(20);
    
    //Pridani prniho prvku
    ASSERT_TRUE(singleNode.first);
    EXPECT_EQ(singleNode.second->key, 20);
    EXPECT_TRUE(tree.GetRoot()==singleNode.second);

    //Pridani jiz existujiciho prvku
    std::pair<bool, BinaryTree::Node_t *> singleNode2 = tree.InsertNode(20);
    EXPECT_FALSE(singleNode2.first);
    EXPECT_EQ(singleNode2.second->key, singleNode.second->key);

    //Pridani noveho prvku
    std::pair<bool, BinaryTree::Node_t *> singleNode3 = tree.InsertNode(10);
    EXPECT_TRUE(singleNode3.first);
    EXPECT_EQ(singleNode3.second->key, 10);
    EXPECT_NE(singleNode3.second->key, singleNode.second->key);
    EXPECT_FALSE(tree.GetRoot()==singleNode3.second);

    
}

TEST_F(EmptyTree, DeleteNode)
{
    //Mazani neexistujiciho prvku
    ASSERT_FALSE(tree.DeleteNode(20));
    EXPECT_TRUE(tree.GetRoot()==NULL);

    //Mazani zalozeneho prvku
    std::pair<bool, BinaryTree::Node_t *> singleNode = tree.InsertNode(20);
    EXPECT_TRUE(tree.GetRoot() != NULL);
    EXPECT_TRUE(tree.DeleteNode(20));
    EXPECT_TRUE(tree.GetRoot() == NULL);

    //Mazani jiz smazaneho prvku
    EXPECT_FALSE(tree.DeleteNode(20));

    //Mazani nahodnych hodnot
    int values[] = { 10, 85, 15, 70, 19, 60};

    for (int i = 0; i < 6; i++)
    {
        EXPECT_FALSE(tree.DeleteNode(values[i]));
    }
    //Kontrola, ze neexistuje root
    EXPECT_TRUE(tree.GetRoot() == NULL);
}

TEST_F(EmptyTree, FindNode)
{
    //Hledani neexistujiciho prvku
    ASSERT_TRUE(tree.FindNode(20) == NULL);

    //Hledani existujiciho prvku
    std::pair<bool, BinaryTree::Node_t *> singleNode = tree.InsertNode(20);
    EXPECT_FALSE(tree.FindNode(20) == NULL);

    //Hledani nahodnych hodnot
    int values[] = { 10, 85, 15, 70, 19, 60};

    for (int i = 0; i < 6; i++)
    {
        EXPECT_TRUE(tree.FindNode(values[i]) == NULL);
    }
}

TEST_F(NonEmptyTree, InsertNode)
{
    //Pridani prvku, ktery neni ve stromu
    ASSERT_TRUE(tree.GetRoot()!= NULL);
    std::pair<bool, BinaryTree::Node_t *> singleNode = tree.InsertNode(20);
    EXPECT_TRUE(singleNode.first);
    EXPECT_EQ(singleNode.second->key, 20);
    EXPECT_FALSE(singleNode.second->pParent == NULL);
    EXPECT_FALSE(singleNode.second->pLeft == NULL);
    EXPECT_EQ(singleNode.second->pParent->color, BinaryTree::BLACK);
    EXPECT_EQ(singleNode.second->color, BinaryTree::RED);


    //Pridani prvku, ktery existuje ve stromu
    std::pair<bool, BinaryTree::Node_t *> singleNode2 = tree.InsertNode(19);
    EXPECT_FALSE(singleNode2.first);
    EXPECT_EQ(singleNode2.second->key, 19);
    EXPECT_FALSE(singleNode2.second->pParent == NULL);
    EXPECT_TRUE(singleNode2.second->color == BinaryTree::RED);

    //Pridani prvku, ktery byl pridan testem
    std::pair<bool, BinaryTree::Node_t *> singleNode3 = tree.InsertNode(20);
    EXPECT_FALSE(singleNode3.first);
    EXPECT_EQ(singleNode3.second->key, singleNode.second->key);
    EXPECT_TRUE(singleNode3.second->pParent == singleNode.second->pParent);

    //Pridani zaporneho prvku
    std::pair<bool, BinaryTree::Node_t *> singleNodeNEG = tree.InsertNode(-1);
    EXPECT_TRUE(singleNodeNEG.first);
    EXPECT_FALSE(singleNodeNEG.second->pParent == NULL);
    EXPECT_EQ(singleNodeNEG.second->key, -1);
    EXPECT_EQ(singleNodeNEG.second->pParent->key, 5);

    //Pridani prvku s hodnotou root
    std::pair<bool, BinaryTree::Node_t *> singleNodeMS = tree.InsertNode(50);
    EXPECT_FALSE(singleNodeMS.first);
    EXPECT_TRUE(tree.GetRoot()==singleNodeMS.second);
    EXPECT_TRUE(singleNodeMS.second->pParent == NULL);
    EXPECT_EQ(singleNodeMS.second->color, BinaryTree::BLACK);
    EXPECT_EQ(singleNodeMS.second->pLeft->key, 19);
    EXPECT_EQ(singleNodeMS.second->pRight->key, 70);

}

TEST_F(NonEmptyTree, DeleteNode)
{
    //Smazani prvku ze seznamu
    ASSERT_TRUE(tree.DeleteNode(15));

    //smazani pridaneho prvku
    std::pair<bool, BinaryTree::Node_t *> singleNode3 = tree.InsertNode(20);
    EXPECT_TRUE(tree.DeleteNode(20));
    
    //Smazani ostatnich prvku seznamu v jinem poradi
    int values[] = {  90, 40, 5, 55, 10, 85, 70, 19, 60, 30, 50, 65, 80 };
    for (size_t i = 0; i < 13; i++)
    {
        EXPECT_TRUE(tree.DeleteNode(values[i]));
    }

    //Smazani jiz smazanych prvku
        for (size_t i = 0; i < 13; i++)
    {
        EXPECT_FALSE(tree.DeleteNode(values[i]));
    }
}

TEST_F(NonEmptyTree, FindNode)
{
    //Hledani prvku, co je v rade
    ASSERT_TRUE(tree.FindNode(55));
    
    //Hledani pridaneho prvku
    std::pair<bool, BinaryTree::Node_t *> singleNode = tree.InsertNode(158);
    EXPECT_TRUE(tree.FindNode(158));
    
    //Hledani smazaneho prvku
    tree.DeleteNode(158);
    EXPECT_FALSE(tree.FindNode(158));

    //Hledani vsech cisel v rade
    int values[] = { 50, 85, 15, 70, 19, 60, 30, 10, 65, 80, 90, 40, 5, 55 };
    for (size_t i = 0; i < 14; i++)
    {
        EXPECT_TRUE(tree.FindNode(values[i]));
    }

    //smazani vsech prvku a nasledne hledani smazaneho prvku
    for (size_t i = 0; i < 14; i++)
    {
        tree.DeleteNode(values[i]);
    }
    for (size_t i = 0; i < 14; i++)
    {
        EXPECT_FALSE(tree.FindNode(values[i]));
    }
}

TEST_F(TreeAxioms, Axiom1)
{
    std::vector<BinaryTree::Node_t *> Node;
    tree.GetLeafNodes(Node);

    //cyklus projde vsechny listy a prekontroluje jejich barvu
    for (BinaryTree::Node_t *tmp : Node)
    {
        ASSERT_EQ(tmp->color, BinaryTree::BLACK);
    }
    
}

TEST_F(TreeAxioms, Axiom2)
{
    std::vector<BinaryTree::Node_t *> Node;
    tree.GetAllNodes(Node);

    //cyklus co projde vsemi nody
    for(BinaryTree::Node_t *tmp : Node)
    {
        //pri splneni podminky cerveneho noudu vyhodnoti test
        if (tmp->color == BinaryTree::RED)
        {
            ASSERT_EQ(tmp->pLeft->color, BinaryTree::BLACK);
            ASSERT_EQ(tmp->pLeft->color, BinaryTree::BLACK);
        }
    }
}

TEST_F(TreeAxioms, axiom3)
{   
    //ziskani listu
    std::vector<BinaryTree::Node_t *> LeafNode;
    tree.GetLeafNodes(LeafNode);

    //ziskani stromu
    std::vector<BinaryTree::Node_t *> Node;
    tree.GetAllNodes(Node);

    int totalNodesCount = sizeof(Node);
    int blackNodesCount[totalNodesCount];
    int count = 0;
    int branchCount = 0;

    //Cyklus co projede cely strom
    for(BinaryTree::Node_t *tmp : LeafNode)
    {
        //Dokud nema tmp rodice
        while (tmp->pParent != NULL)
        {
            if(tmp->color == BinaryTree::BLACK)
            {
                count++;
            }
            tmp = tmp->pParent;
        }
        //nastaveni hodnoty na poradi vetve
        blackNodesCount[branchCount] = count;
        //presun na nasledujici vetev
        branchCount++;
        //vynulovani poctu 
        count = 0;
    }

    for (size_t i = 0; i < branchCount; i++)
    {  
        for (size_t j = 0; j < branchCount; j++)
        {
            //Aby se neporovnvala kazda vetev sama se sebou
            if (i != j)
            {
                EXPECT_EQ(blackNodesCount[i], blackNodesCount[j]);   
            }
        }
    }
    
}

/*** Konec souboru black_box_tests.cpp ***/
