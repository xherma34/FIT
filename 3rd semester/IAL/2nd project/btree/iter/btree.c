/*
 * Binárny vyhľadávací strom — iteratívna varianta
 *
 * S využitím dátových typov zo súboru btree.h, zásobníkov zo súborov stack.h a
 * stack.c a pripravených kostier funkcií implementujte binárny vyhľadávací
 * strom bez použitia rekurzie.
 */

#include "../btree.h"
#include "stack.h"
#include <stdio.h>
#include <stdlib.h>

/*
 * Inicializácia stromu.
 *
 * Užívateľ musí zaistiť, že incializácia sa nebude opakovane volať nad
 * inicializovaným stromom. V opačnom prípade môže dôjsť k úniku pamäte (memory
 * leak). Keďže neinicializovaný ukazovateľ má nedefinovanú hodnotu, nie je
 * možné toto detegovať vo funkcii.
 */
void bst_init(bst_node_t **tree) {
  (*tree) = NULL;
}

/*
 * Nájdenie uzlu v strome.
 *
 * V prípade úspechu vráti funkcia hodnotu true a do premennej value zapíše
 * hodnotu daného uzlu. V opačnom prípade funckia vráti hodnotu false a premenná
 * value ostáva nezmenená.
 *
 * Funkciu implementujte iteratívne bez použitia vlastných pomocných funkcií.
 */
bool bst_search(bst_node_t *tree, char key, int *value) {
  if(tree != NULL)
  {
    while(tree->key != key)
    {
      if(key > tree->key)
      {
        if(tree->right == NULL)
        {
          return false;
        }
        tree = tree->right;
      }
      else if(key < tree->key)
      {
        if(tree->left == NULL)
        {
          return false;
        }
        tree = tree->left;
      }
    }
    *value = tree->key;
    return true;
  }
  return false;
}

/*
 * Vloženie uzlu do stromu.
 *
 * Pokiaľ uzol so zadaným kľúčom v strome už existuje, nahraďte jeho hodnotu.
 * Inak vložte nový listový uzol.
 *
 * Výsledný strom musí spĺňať podmienku vyhľadávacieho stromu — ľavý podstrom
 * uzlu obsahuje iba menšie kľúče, pravý väčšie.
 *
 * Funkciu implementujte iteratívne bez použitia vlastných pomocných funkcií.
 */
void bst_insert(bst_node_t **tree, char key, int value) {

  bst_node_t *curr = *tree;   //promenna pro posouvani
  bst_node_t *insertMe = malloc(sizeof(bst_node_t));    //prvek, ktery budeme insertovat
  insertMe->right = insertMe->left = NULL;     //inicializace
  insertMe->value = value;
  insertMe->key = key;
  bst_node_t *parent = NULL;    //Rodicovsky prvek(prvek za ktery budeme insertovat)

  while(curr)   //Dokud se mame kam posouvat => curr!=NULL
  {
    parent = curr;    //Starnuti otcovskeho prvku

    if(key < curr->key)   //pokud je klic mensi => posun doleva
    {
      curr = curr->left;
    }
    else if(key > curr->key)    //pokud je klic vetsi => posun doprava
    {
      curr = curr->right;
    }
    else    //pokud se klic rovna => prepisu hodnotu
    {
      curr->value = value;
      free(insertMe);
      return;
    }
  }

  if(parent == NULL)    //Strom je prazdny
  {
    parent = insertMe;    //Insertovany prvek bude rodicem 
    *tree = parent;
    return;
  }
  else if(key < parent->key)    //POkud je rodicuv klic vetsi 
  {
    parent->left = insertMe;    //insertuji doleva od rodice
    return;
  }
  else if(key > parent->key)    //POkud je rodicuv klic mensi
  {
    parent->right = insertMe;   //insertuji doprava od rodice
    return;
  }
  

}

/*
 * Pomocná funkcia ktorá nahradí uzol najpravejším potomkom.
 *
 * Kľúč a hodnota uzlu target budú nahradené kľúčom a hodnotou najpravejšieho
 * uzlu podstromu tree. Najpravejší potomok bude odstránený. Funkcia korektne
 * uvoľní všetky alokované zdroje odstráneného uzlu.
 *
 * Funkcia predpokladá že hodnota tree nie je NULL.
 *
 * Táto pomocná funkcia bude využitá pri implementácii funkcie bst_delete.
 *
 * Funkciu implementujte iteratívne bez použitia vlastných pomocných funkcií.
 */
void bst_replace_by_rightmost(bst_node_t *target, bst_node_t **tree) {

  bst_node_t *parent = NULL;
  bst_node_t *curr = *tree;
  target->left = *tree;
  while(curr->right != NULL)
  {
    parent = curr;
    curr = curr->right;   //Do replace ulozim uplne pravy prvek
  }
  
  target->key = curr->key;
  target->value = curr->value;

  if(parent != NULL)
  {
    parent->right = curr->left;
  }
    free(curr);
}

/*
 * Odstránenie uzlu v strome.
 *
 * Pokiaľ uzol so zadaným kľúčom neexistuje, funkcia nič nerobí.
 * Pokiaľ má odstránený uzol jeden podstrom, zdedí ho otec odstráneného uzla.
 * Pokiaľ má odstránený uzol oba podstromy, je nahradený najpravejším uzlom
 * ľavého podstromu. Najpravejší uzol nemusí byť listom!
 * Funkcia korektne uvoľní všetky alokované zdroje odstráneného uzlu.
 *
 * Funkciu implementujte iteratívne pomocou bst_replace_by_rightmost a bez
 * použitia vlastných pomocných funkcií.
 */
void bst_delete(bst_node_t **tree, char key) {

  bst_node_t *curr = *tree;   //posouvaci promenna
  bst_node_t *parent = NULL;  //uchovavatel rodicovskeho uzlu

  while(curr != NULL && curr->key != key)   //Divame se zda hledany key je vubec v nasem stromu
  {
    parent = curr;    //starnuti
    if(key < curr->key)   //pokud je klic mensi => posun doleva
    {
      curr = curr->left;
    }
    else    //pokud je klic vetsi => posun doprava
    {
      curr = curr->right;
    }
  }

  if(curr == NULL)    //Hledany klic neni v nasem stromu
  {
    return;
  }
  
  //Pokud ma mazany prvek maximalne 1. potomka
  if(curr->left == NULL || curr->right == NULL)   
  {
    bst_node_t *replace = NULL;   //Nahradi odstraneny prvek
    if(curr->left == NULL)    //Pokud mazany prvek nema levy ukazatel
    {
      replace = curr->right;    //Nema leveho, ale ma praveho a ten nahradim za mazany prvek
    }
    else    //POkud mazany prvek nema pravy ukazatel
    {   
      replace = curr->left;   //Nema praveho, ale ma leveho a ten nahradim za mazany prvek
    }

    if(parent == NULL)    //Pokud mazany prvek je korenem stromu
    {
      *tree = replace;
    }

    if(curr == parent->left)    //POkud je mazany prvek levy potomek sveho otce
    {
      parent->left = replace;   //nahradim levy
    }
    else    //Pokud je mazany prvek pravy potomek sveho otce
    {
      parent->right = replace;    //nahradim pravy
    }
    free(curr);   //Uvolnim pamet mazaneho
  }
  //Pokud ma dva potomky
  else
  {
    bst_replace_by_rightmost(curr, &curr->left);
  }
  return;

}

/*
 * Zrušenie celého stromu.
 *
 * Po zrušení sa celý strom bude nachádzať v rovnakom stave ako po
 * inicializácii. Funkcia korektne uvoľní všetky alokované zdroje rušených
 * uzlov.
 *
 * Funkciu implementujte iteratívne pomocou zásobníku uzlov a bez použitia
 * vlastných pomocných funkcií.
 */
void bst_dispose(bst_node_t **tree) {

  stack_bst_t s1;
  stack_bst_init(&s1);
  
  //Funkce implementovana podle prednasky IAL 2021 

  do 
  {
    if (*tree == NULL)
    { 
      if (!stack_bst_empty(&s1)) //Pokud mam vyprazdneny strom a vsechny prvky jsou na zasobniku
      {
        *tree = stack_bst_top(&s1);   //do tree si ulozim top
        stack_bst_pop(&s1);   //popnu ho
      }
    } 
    else 
    {
      if ((*tree)->right != NULL)   //Pravy potomek dam na zasobnik
      { 
        stack_bst_push(&s1,(*tree)->right);
      }
      bst_node_t *auxPtr = *tree;   //pomocna promenna
      *tree = (*tree)->left;    //posunu se doleva
      free(auxPtr);   //uvolnim aktualni prvek
    } 
  } while(((*tree) != NULL)||(!stack_bst_empty(&s1)));

}

/*
 * Pomocná funkcia pre iteratívny preorder.
 *
 * Prechádza po ľavej vetve k najľavejšiemu uzlu podstromu.
 * Nad spracovanými uzlami zavola bst_print_node a uloží ich do zásobníku uzlov.
 *
 * Funkciu implementujte iteratívne pomocou zásobníku uzlov a bez použitia
 * vlastných pomocných funkcií.
 */
void bst_leftmost_preorder(bst_node_t *tree, stack_bst_t *to_visit) {
  
  
  while(tree != NULL)
  {
    stack_bst_push(to_visit, tree);
    bst_print_node(tree);
    tree = tree->left;
  }

}

/*
 * Preorder prechod stromom.
 *
 * Pre aktuálne spracovávaný uzol nad ním zavolajte funkciu bst_print_node.
 *
 * Funkciu implementujte iteratívne pomocou funkcie bst_leftmost_preorder a
 * zásobníku uzlov bez použitia vlastných pomocných funkcií.
 */
void bst_preorder(bst_node_t *tree) {
  stack_bst_t stack;
  stack_bst_init(&stack);
  bst_leftmost_preorder(tree, &stack);
  while(!stack_bst_empty(&stack))
  {
    tree = stack_bst_top(&stack);
    stack_bst_pop(&stack);
    bst_leftmost_preorder(tree->right, &stack);
  }

}

/*
 * Pomocná funkcia pre iteratívny inorder.
 *
 * Prechádza po ľavej vetve k najľavejšiemu uzlu podstromu a ukladá uzly do
 * zásobníku uzlov.
 *
 * Funkciu implementujte iteratívne pomocou zásobníku uzlov a bez použitia
 * vlastných pomocných funkcií.
 */
void bst_leftmost_inorder(bst_node_t *tree, stack_bst_t *to_visit) {
  while(tree != NULL)
  {
    stack_bst_push(to_visit, tree);
    tree = tree->left;
  }
}

/*
 * Inorder prechod stromom.
 *
 * Pre aktuálne spracovávaný uzol nad ním zavolajte funkciu bst_print_node.
 *
 * Funkciu implementujte iteratívne pomocou funkcie bst_leftmost_inorder a
 * zásobníku uzlov bez použitia vlastných pomocných funkcií.
 */
void bst_inorder(bst_node_t *tree) {
  stack_bst_t stack;
  stack_bst_init(&stack);
  bst_leftmost_inorder(tree, &stack);
  while(!stack_bst_empty(&stack))
  {
    tree = stack_bst_top(&stack);
    stack_bst_pop(&stack);
    bst_print_node(tree);
    bst_leftmost_inorder(tree->right, &stack);
  }
}

/*
 * Pomocná funkcia pre iteratívny postorder.
 *
 * Prechádza po ľavej vetve k najľavejšiemu uzlu podstromu a ukladá uzly do
 * zásobníku uzlov. Do zásobníku bool hodnôt ukladá informáciu že uzol
 * bol navštívený prvý krát.
 *
 * Funkciu implementujte iteratívne pomocou zásobníkov uzlov a bool hodnôt a bez použitia
 * vlastných pomocných funkcií.
 */
void bst_leftmost_postorder(bst_node_t *tree, stack_bst_t *to_visit, stack_bool_t *first_visit) {
  while(tree != NULL)
  {
    stack_bst_push(to_visit, tree);
    stack_bool_push(first_visit, true);
    tree = tree->left;
  }
}

/*
 * Postorder prechod stromom.
 *
 * Pre aktuálne spracovávaný uzol nad ním zavolajte funkciu bst_print_node.
 *
 * Funkciu implementujte iteratívne pomocou funkcie bst_leftmost_postorder a
 * zásobníkov uzlov a bool hodnôt bez použitia vlastných pomocných funkcií.
 */
void bst_postorder(bst_node_t *tree) {
  bool fromLeft;
  stack_bst_t stack;
  stack_bst_init(&stack);
  stack_bool_t stackBool;
  stack_bool_init(&stackBool);
  bst_leftmost_postorder(tree, &stack, &stackBool);

  while(!stack_bst_empty(&stack))
  {
    tree = stack_bst_top(&stack);
    fromLeft = stack_bool_top(&stackBool);
    stack_bool_pop(&stackBool);
    if(fromLeft)
    {
      stack_bool_push(&stackBool, false);
      bst_leftmost_postorder(tree->right, &stack, &stackBool);
    }
    else
    {
      stack_bst_pop(&stack);
      bst_print_node(tree);
    }
  }

}
