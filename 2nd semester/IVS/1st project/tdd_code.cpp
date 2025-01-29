//======== Copyright (c) 2021, FIT VUT Brno, All rights reserved. ============//
//
// Purpose:     Test Driven Development - priority queue code
//
// $NoKeywords: $ivs_project_1 $tdd_code.cpp
// $Author:     Pavel Heřmann <xherma34@stud.fit.vutbr.cz>
// $Date:       $2021-03-03
//============================================================================//
/**
 * @file tdd_code.cpp
 * @author Pavel Heřmann
 * 
 * @brief Implementace metod tridy prioritni fronty.
 */

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include "tdd_code.h"

//============================================================================//
// ** ZDE DOPLNTE IMPLEMENTACI **
//
// Zde doplnte implementaci verejneho rozhrani prioritni fronty (Priority Queue)
// 1. Verejne rozhrani fronty specifikovane v: tdd_code.h (sekce "public:")
//    - Konstruktor (PriorityQueue()), Destruktor (~PriorityQueue())
//    - Metody Insert/Remove/Find a GetHead
//    - Pripadne vase metody definovane v tdd_code.h (sekce "protected:")
//
// Cilem je dosahnout plne funkcni implementace prioritni fronty implementovane
// pomoci tzv. "double-linked list", ktera bude splnovat dodane testy 
// (tdd_tests.cpp).
//============================================================================//

PriorityQueue::PriorityQueue()
{
    m_pHead = nullptr;
}

PriorityQueue::~PriorityQueue()
{
    Element_t *tmp = m_pHead;
    
    while (tmp != NULL)
    {
        //Holder abychom se mohli po smazani presunout dal
        Element_t *hold = tmp;
        tmp = tmp->pNext;
        delete hold;
    }
    
}

void PriorityQueue::Insert(int value)
{

    //hlava je prazdna
    if (m_pHead == nullptr)
    {
        m_pHead = new Element_t
        {
            .pNext = nullptr,
            .value = value
        };
    }

    //hlava neni prazdna
    else
    {
        //place, vytvori se vzdy pro ucel zarazovani na pozici
        Element_t *place = new Element_t
        {
            .pNext = nullptr,
            .value = value
        };
        //prochazeni celeho queue
        for (Element_t *tmp = m_pHead; tmp != nullptr; tmp = tmp->pNext)
        {   
            //prvek je vetsi nez hlava
            if (m_pHead->value < value)
            {
                //vytvorim hlavu, nastavim ukazatel na current polozku
                m_pHead = new Element_t
                {
                    .pNext = tmp,
                    .value = place->value
                };
                return;
            }
            //Zarazovani prvku, ktery neni predposledni do poradi MAX-> MIN
            else if(tmp->pNext != nullptr && tmp->pNext->value <= value)
            {
                place->pNext = tmp->pNext;
                tmp->pNext = place;
                return;
            }
            //Zarazovani na konec rady
            else if(tmp->pNext == nullptr)
            {
                tmp->pNext = place;
                return;
            }
        }
    }
}

bool PriorityQueue::Remove(int value)
{

    Element_t *previous;
    //cyklem projedu celou radu
    for (Element_t *tmp = m_pHead; tmp != nullptr; tmp = tmp->pNext)
    {
        //Prvek, ktery chceme mazat je prvni
       if (value == m_pHead->value)
       {
            //Rada uz je serazena, takze m_pHead bude nasledujici prvek
            m_pHead = tmp->pNext;
            //Prvek smazme
            tmp = NULL;
            return true;
       }
       //Prvek, ktery chceme mazat je posledni
       else if (value == tmp->value && tmp->pNext == nullptr)
       {
           previous->pNext = nullptr;
           tmp = NULL;
           return true;
       }
       //Prvek, ktery chceme mazat neni posledni ani prvni
       else if (tmp->value == value)
       {
           previous->pNext = tmp->pNext;
           tmp = NULL;
           return true;
       }
       
       previous = tmp;
    }
    return false;
}

PriorityQueue::Element_t *PriorityQueue::Find(int value)
{
    

    for (Element_t *tmp = m_pHead; tmp != nullptr; tmp = tmp->pNext)
    {
        if (tmp->value == value)
        {
            return tmp;
        }
    }
    return NULL;

}

size_t PriorityQueue::Length()
{
    int count = 0;

    for (Element_t *tmp = m_pHead; tmp != nullptr; tmp = tmp->pNext)
    {
        count++;
    }
    

    return count;
	
}

PriorityQueue::Element_t *PriorityQueue::GetHead()
{
    return m_pHead;
}

/*** Konec souboru tdd_code.cpp ***/
