/*
 * Tabuľka s rozptýlenými položkami
 *
 * S využitím dátových typov zo súboru hashtable.h a pripravených kostier
 * funkcií implementujte tabuľku s rozptýlenými položkami s explicitne
 * zreťazenými synonymami.
 *
 * Pri implementácii uvažujte veľkosť tabuľky HT_SIZE.
 */

#include "hashtable.h"
#include <stdlib.h>
#include <string.h>

int HT_SIZE = MAX_HT_SIZE;

/*
 * Rozptyľovacia funkcia ktorá pridelí zadanému kľúču index z intervalu
 * <0,HT_SIZE-1>. Ideálna rozptyľovacia funkcia by mala rozprestrieť kľúče
 * rovnomerne po všetkých indexoch. Zamyslite sa nad kvalitou zvolenej funkcie.
 */
int get_hash(char *key) {
  int result = 1;
  int length = strlen(key);
  for (int i = 0; i < length; i++) {
    result += key[i];
  }
  return (result % HT_SIZE);
}

/*
 * Inicializácia tabuľky — zavolá sa pred prvým použitím tabuľky.
 */
void ht_init(ht_table_t *table) {
  for (int i = 0; i < HT_SIZE; i++)
  {
    (*table)[i] = NULL;
  }
}

/*
 * Vyhľadanie prvku v tabuľke.
 *
 * V prípade úspechu vráti ukazovateľ na nájdený prvok; v opačnom prípade vráti
 * hodnotu NULL.
 */
ht_item_t *ht_search(ht_table_t *table, char *key) {
  
  ht_item_t *item = (*table)[get_hash(key)];
  while (item)
  {
    if(strcmp(key, item->key) == 0) return item;
    item = item->next;
  }
  return NULL;
}

/*
 * Vloženie nového prvku do tabuľky.
 *
 * Pokiaľ prvok s daným kľúčom už v tabuľke existuje, nahraďte jeho hodnotu.
 *
 * Pri implementácii využite funkciu ht_search. Pri vkladaní prvku do zoznamu
 * synonym zvoľte najefektívnejšiu možnosť a vložte prvok na začiatok zoznamu.
 */
void ht_insert(ht_table_t *table, char *key, float value) {
  
  ht_item_t *item = ht_search(table, key);
  

  if(item != NULL)    //Pokud na danem klici je hodnota, tak ji prepisu
  {
    item->value = value;    
    return;
  }
  else    //Pokud na danem klici je synonymum, ale neni tam stejna hodnota
  {
    ht_item_t *insertMe = malloc(sizeof(ht_table_t));   //vytvorim nove vkladany prvek
    insertMe->key = key;    //pripisu mu klic
    insertMe->value = value;    //pripisu mu hodnotu
    insertMe->next = (*table)[get_hash(key)];   //Ukazatel na next nove vkladaneho prvku nastavim na prvni prvek na pozici v hashovaci tabulce
    (*table)[get_hash(key)] = insertMe;     //nastavim nove vkladany prvek na prvni pozici 
  }

}

/*
 * Získanie hodnoty z tabuľky.
 *
 * V prípade úspechu vráti funkcia ukazovateľ na hodnotu prvku, v opačnom
 * prípade hodnotu NULL.
 *
 * Pri implementácii využite funkciu ht_search.
 */
float *ht_get(ht_table_t *table, char *key) {

  ht_item_t *getValue = ht_search(table, key);

  if((*table)[get_hash(key)] != NULL)
  {
    return &getValue->value;
  }
  else
  {
    return NULL;
  }
  
}

/*
 * Zmazanie prvku z tabuľky.
 *
 * Funkcia korektne uvoľní všetky alokované zdroje priradené k danému prvku.
 * Pokiaľ prvok neexistuje, nerobte nič.
 *
 * Pri implementácii NEVYUŽÍVAJTE funkciu ht_search.
 */
void ht_delete(ht_table_t *table, char *key) {

  ht_item_t *deleteMe = (*table)[get_hash(key)];  //hledany prvek
  ht_item_t *previous;
  if(!deleteMe) return;

  while(deleteMe != NULL)
  {
    
    if(strcmp(key, deleteMe->key) == 0)   //Pokud najdeme shodu klice
    {
      if(previous == NULL)    //kdyz mazu prvni prvek
      {
        (*table)[get_hash(key)] = deleteMe->next;
        free(deleteMe);
        return;
      }
      else
      {
        previous->next = deleteMe->next;
        free(deleteMe);
        return;
      }
    }

    previous = deleteMe;
    deleteMe = deleteMe->next;
  }

}

/*
 * Zmazanie všetkých prvkov z tabuľky.
 *
 * Funkcia korektne uvoľní všetky alokované zdroje a uvedie tabuľku do stavu po
 * inicializácii.
 */
void ht_delete_all(ht_table_t *table) {
  ht_item_t *line;
  ht_item_t *node;
  
  for (int i = 0; i < HT_SIZE; i++)
  {
    line = (*table)[i];

    while(line != NULL)
    {
      node = line;
      line = line->next;
      free(node);
    }
    (*table)[i] = NULL;
  }
  
}
