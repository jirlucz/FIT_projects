//======== Copyright (c) 2022, FIT VUT Brno, All rights reserved. ============//
//
// Purpose:     White Box - test suite
//
// $NoKeywords: $ivs_project_1 $white_box_tests.cpp
// $Author:     Jiri Kotek <xkotek09@stud.fit.vutbr.cz>
// $Date:       $2023-03-07
//============================================================================//
/**
 * @file white_box_tests.cpp
 * @author Jiri Kotek
 * 
 * @brief Implementace testu hasovaci tabulky.
 */

#include <vector>

#include "gtest/gtest.h"

#include "white_box_code.h"

//============================================================================//
// ** ZDE DOPLNTE TESTY **
//
// Zde doplnte testy hasovaci tabulky, testujte nasledujici:
// 1. Verejne rozhrani hasovaci tabulky
//     - Vsechny funkce z white_box_code.h
//     - Chovani techto metod testuje pro prazdnou i neprazdnou tabulku.
// 2. Chovani tabulky v hranicnich pripadech
//     - Otestujte chovani pri kolizich ruznych klicu se stejnym hashem 
//     - Otestujte chovani pri kolizich hashu namapovane na stejne misto v 
//       indexu
//============================================================================//

class EmptyMap : public :: testing :: Test{

protected:
    hash_map* map = hash_map_ctor();
};

class NonEmptyMap : public :: testing :: Test{

protected: 
    hash_map* map = hash_map_ctor();
    virtual void SetUp (){ 

        
        ASSERT_EQ (hash_map_put (map, "palma", 42), OK );
        ASSERT_EQ (hash_map_put (map, "orchidej", 672), OK );
        ASSERT_EQ (hash_map_put (map, "aabbcc", 61), OK );
        ASSERT_EQ (hash_map_put (map, "dddeskaa", 246), OK );
        ASSERT_EQ (hash_map_put (map, "Tigerrr", 7), OK );
        ASSERT_EQ (hash_map_put (map, "TigerII", 157), OK );
    };
};


TEST_F(EmptyMap, MapConstructor){
     ASSERT_NE(map, nullptr);

    EXPECT_NE(map, nullptr);
    EXPECT_EQ(map->used , 0);
    EXPECT_EQ(map->allocated , 8);

    hash_map_dtor(map);
}

TEST_F(NonEmptyMap, MapConstructor){
    ASSERT_NE(map, nullptr);

    EXPECT_EQ(map->used , 6);
    EXPECT_EQ (hash_map_size(map), 6);
    EXPECT_EQ(hash_map_capacity(map) , 16);

    hash_map_dtor(map);
}

TEST_F(EmptyMap, FindKey){
    ASSERT_NE(map, nullptr);
    
    auto Key1 = hash_map_put(map, "teplomer", 78);
    EXPECT_EQ(Key1, OK );

    auto Key1Find = hash_map_contains(map, "teplomer");
    EXPECT_NE(Key1Find, 0);

    auto Key2Find = hash_map_contains(map, "zlato");
    EXPECT_EQ(Key2Find, 0);

    hash_map_remove(map, "teplomer");
    auto Key1AfterDel = hash_map_contains(map, "teplomer");
    EXPECT_EQ(Key1AfterDel, 0);

    hash_map_dtor(map);
}

TEST_F(NonEmptyMap, FindKey){
    ASSERT_NE(map, nullptr);

    auto Key1Find = hash_map_contains(map, "palma");
    EXPECT_NE(Key1Find, 0);

    auto Key2Find = hash_map_contains(map, "TigerII");
    EXPECT_NE(Key2Find, 0);

    auto Key3Find = hash_map_contains(map, "007");
    EXPECT_EQ(Key3Find, 0);

    hash_map_dtor(map);
}

TEST_F(EmptyMap, DeleteKey){
    ASSERT_NE(map, nullptr);
    auto KeyRemove = hash_map_remove(map, "klavesnice");
    EXPECT_EQ (KeyRemove, KEY_ERROR);

    hash_map_dtor(map);
}

TEST_F(NonEmptyMap, DeleteKey){
    ASSERT_NE(map, nullptr);
    ASSERT_EQ (hash_map_contains(map, "dddeskaa"), true );

    EXPECT_EQ (hash_map_contains(map, "palma"), true );
    EXPECT_EQ (hash_map_contains(map, "orchidej"), true );
    EXPECT_EQ (hash_map_contains(map, "aabbcc"), true );
    EXPECT_EQ (hash_map_contains(map, "dddeskaa"), true );
    EXPECT_EQ (hash_map_contains(map, "Tigerrr"), true );
    EXPECT_EQ (hash_map_contains(map, "TigerII"), true );
    
    auto RemoveNode = hash_map_remove(map, "aabbcc");
    EXPECT_EQ (RemoveNode, OK);

    auto RemoveNode2 = hash_map_remove(map, "aabbcc");
    EXPECT_EQ (RemoveNode2, KEY_ERROR);

    hash_map_put (map, "aabbcc", 61);

    auto RemoveNode3 = hash_map_remove(map, "aabbcc");
    EXPECT_EQ (RemoveNode3, OK);

    hash_map_dtor(map);
}

TEST_F(EmptyMap, KeyPop){
    ASSERT_NE(map, nullptr);
    
    int value;
    auto KeyPop = hash_map_get(map, "eegyuuo", &value);
    EXPECT_EQ (KeyPop, KEY_ERROR);

    hash_map_dtor(map);
}

TEST_F(NonEmptyMap, KeyPop){
    ASSERT_NE(map, nullptr);
    
    int value;
    auto KeyPop = hash_map_get(map, "palma", &value);
    EXPECT_EQ (value, 42);
    EXPECT_EQ (KeyPop, OK);


    hash_map_dtor(map);
}

TEST_F(NonEmptyMap, HashReserveLess){
    ASSERT_NE(map, nullptr);
    
    EXPECT_EQ(hash_map_reserve(map, 4), VALUE_ERROR );

    hash_map_dtor(map);
}

TEST_F(NonEmptyMap, HashReserveSame){
    ASSERT_NE(map, nullptr);
    
    EXPECT_EQ(hash_map_reserve(map, 16), OK );

    hash_map_dtor(map);
}

TEST_F(NonEmptyMap, InsertSameKey){
    ASSERT_NE(map, nullptr);
    
    auto Input = hash_map_put (map, "aabbcc", 61);
    EXPECT_EQ(Input, KEY_ALREADY_EXISTS);

    hash_map_dtor(map);
}





/*** Konec souboru white_box_tests.cpp ***/
