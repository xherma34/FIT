
/* ******************************* c206.c *********************************** */
/*  Předmět: Algoritmy (IAL) - FIT VUT v Brně                                 */
/*  Úkol: c206 - Dvousměrně vázaný lineární seznam                            */
/*  Návrh a referenční implementace: Bohuslav Křena, říjen 2001               */
/*  Vytvořil: Martin Tuček, říjen 2004                                        */
/*  Upravil: Kamil Jeřábek, září 2020                                         */
/*           Daniel Dolejška, září 2021                                       */
/* ************************************************************************** */
/*
** Implementujte abstraktní datový typ dvousměrně vázaný lineární seznam.
** Užitečným obsahem prvku seznamu je hodnota typu int. Seznam bude jako datová
** abstrakce reprezentován proměnnou typu DLList (DL znamená Doubly-Linked
** a slouží pro odlišení jmen konstant, typů a funkcí od jmen u jednosměrně
** vázaného lineárního seznamu). Definici konstant a typů naleznete
** v hlavičkovém souboru c206.h.
**
** Vaším úkolem je implementovat následující operace, které spolu s výše
** uvedenou datovou částí abstrakce tvoří abstraktní datový typ obousměrně
** vázaný lineární seznam:
**
**      DLL_Init ........... inicializace seznamu před prvním použitím,
**      DLL_Dispose ........ zrušení všech prvků seznamu,
**      DLL_InsertFirst .... vložení prvku na začátek seznamu,
**      DLL_InsertLast ..... vložení prvku na konec seznamu,
**      DLL_First .......... nastavení aktivity na první prvek,
**      DLL_Last ........... nastavení aktivity na poslední prvek,
**      DLL_GetFirst ....... vrací hodnotu prvního prvku,
**      DLL_GetLast ........ vrací hodnotu posledního prvku,
**      DLL_DeleteFirst .... zruší první prvek seznamu,
**      DLL_DeleteLast ..... zruší poslední prvek seznamu,
**      DLL_DeleteAfter .... ruší prvek za aktivním prvkem,
**      DLL_DeleteBefore ... ruší prvek před aktivním prvkem,
**      DLL_InsertAfter .... vloží nový prvek za aktivní prvek seznamu,
**      DLL_InsertBefore ... vloží nový prvek před aktivní prvek seznamu,
**      DLL_GetValue ....... vrací hodnotu aktivního prvku,
**      DLL_SetValue ....... přepíše obsah aktivního prvku novou hodnotou,
**      DLL_Previous ....... posune aktivitu na předchozí prvek seznamu,
**      DLL_Next ........... posune aktivitu na další prvek seznamu,
**      DLL_IsActive ....... zjišťuje aktivitu seznamu.
**
** Při implementaci jednotlivých funkcí nevolejte žádnou z funkcí
** implementovaných v rámci tohoto příkladu, není-li u funkce explicitně
 * uvedeno něco jiného.
**
** Nemusíte ošetřovat situaci, kdy místo legálního ukazatele na seznam
** předá někdo jako parametr hodnotu NULL.
**
** Svou implementaci vhodně komentujte!
**
** Terminologická poznámka: Jazyk C nepoužívá pojem procedura.
** Proto zde používáme pojem funkce i pro operace, které by byly
** v algoritmickém jazyce Pascalovského typu implemenovány jako procedury
** (v jazyce C procedurám odpovídají funkce vracející typ void).
**
**/

#include "c206.h"

int error_flag;
int solved;

/**
 * Vytiskne upozornění na to, že došlo k chybě.
 * Tato funkce bude volána z některých dále implementovaných operací.
 */
void DLL_Error() {
	printf("*ERROR* The program has performed an illegal operation.\n");
	error_flag = TRUE;
}

/**
 * Provede inicializaci seznamu list před jeho prvním použitím (tzn. žádná
 * z následujících funkcí nebude volána nad neinicializovaným seznamem).
 * Tato inicializace se nikdy nebude provádět nad již inicializovaným seznamem,
 * a proto tuto možnost neošetřujte.
 * Vždy předpokládejte, že neinicializované proměnné mají nedefinovanou hodnotu.
 *
 * @param list Ukazatel na strukturu dvousměrně vázaného seznamu
 */
void DLL_Init( DLList *list ) {

    //Nastavime hodnoty seznamu na NULL
    list->activeElement = NULL;
    list->firstElement = NULL;
    list->lastElement = NULL;

}

/**
 * Zruší všechny prvky seznamu list a uvede seznam do stavu, v jakém se nacházel
 * po inicializaci.
 * Rušené prvky seznamu budou korektně uvolněny voláním operace free.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_Dispose( DLList *list ) {

    //Inicializujeme pomocny element sezname
    DLLElementPtr tmp = list->firstElement;
    //Prochazime polem, po iteracich vyhazujeme first element ze seznamu, dokud neni active NULL
    while (tmp != NULL)
    {
        list->firstElement = tmp->nextElement; 
        free(tmp);
        tmp = list->firstElement;
    }
    //Vsechny nody v seznamu opet "nastavime" do pocatecniho stavu po inicializaci
    list->activeElement = NULL;
    list->firstElement = NULL;
    list->lastElement = NULL; 
}

/**
 * Vloží nový prvek na začátek seznamu list.
 * V případě, že není dostatek paměti pro nový prvek při operaci malloc,
 * volá funkci DLL_Error().
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param data Hodnota k vložení na začátek seznamu
 */
void DLL_InsertFirst( DLList *list, int data ) {

    //Vytvorime novy element seznamu a allokujeme mu misto v pameti
    DLLElementPtr tmp = malloc(sizeof(struct DLLElement));

    //Kontrola zda bylo dostatek pameti na allokaci noveho elementu
    if(tmp == NULL)
    {
        DLL_Error();
        return;
    }

    //Prepisu data noveho elementu
    tmp->data = data;

    //Pro neprazdny seznam
    if(list->firstElement != NULL)
    {
        //elementu firstElement dame ukazatel na nas novy prvni element seznamu
        list->firstElement->previousElement = tmp;
        //Novemu elementu dame ukazatel previousElement na NULL, protoze je prvni v listu
        tmp->previousElement = NULL;
        //Next ukazatel mu dame na firstELement
        tmp->nextElement = list->firstElement;
        //Prenastaveni ukazatele firstElement na nove prvni prvek
        list->firstElement = tmp;
    }
    //Pro prazdny seznam
    else
    {
        //Nastavim novy element, jako prvni a posledni v seznamu, protoze jich vice seznam nema
        list->firstElement = tmp;
        list->lastElement = tmp;
    }

}

/**
 * Vloží nový prvek na konec seznamu list (symetrická operace k DLL_InsertFirst).
 * V případě, že není dostatek paměti pro nový prvek při operaci malloc,
 * volá funkci DLL_Error().
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param data Hodnota k vložení na konec seznamu
 */
void DLL_InsertLast( DLList *list, int data ) {

    //Vytvorime novy element seznamu a allokujeme mu misto v pameti
    DLLElementPtr tmp = malloc(sizeof(struct DLLElement));

    //Kontrola zda bylo dostatek pameti na allokaci noveho elementu
    if(tmp == NULL)
    {
        DLL_Error();
        return;
    }

    //Prepisu data noveho elementu a ukazatele noveho elementu
    tmp->data = data;

    //Pro neprazdny seznam
    if(list->firstElement != NULL)
    {
        //elementu lastElement dame ukazatel na nas novy posledni element seznamu
        list->lastElement->nextElement = tmp;
        //Novemu elementu dame ukazatel nextElement na NULL, protoze je posledni v listu
        tmp->nextElement = NULL;
        //Previous ukazatel mu dame na lastElement
        tmp->previousElement = list->lastElement;
        //Prenastaveni ukazatele na posledni element
        list->lastElement = tmp;
    }
    //Pro prazdny seznam
    else
    {
        //Nastavim novy element, jako prvni a posledni v seznamu, protoze jich vice seznam nema
        list->firstElement = tmp;
        list->lastElement = tmp;
    }
}

/**
 * Nastaví první prvek seznamu list jako aktivní.
 * Funkci implementujte jako jediný příkaz (nepočítáme-li return),
 * aniž byste testovali, zda je seznam list prázdný.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_First( DLList *list ) {

    //Nastaveni aktivniho elementu na prvni element
    list->activeElement = list->firstElement;
}

/**
 * Nastaví poslední prvek seznamu list jako aktivní.
 * Funkci implementujte jako jediný příkaz (nepočítáme-li return),
 * aniž byste testovali, zda je seznam list prázdný.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_Last( DLList *list ) {

    //Nastaveni aktivniho elementu na posledni element
    list->activeElement = list->lastElement;
}

/**
 * Prostřednictvím parametru dataPtr vrátí hodnotu prvního prvku seznamu list.
 * Pokud je seznam list prázdný, volá funkci DLL_Error().
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param dataPtr Ukazatel na cílovou proměnnou
 */
void DLL_GetFirst( DLList *list, int *dataPtr ) {

    //Osetreni neprazdnosti seznamu
    if(list->firstElement == NULL)
    {
        DLL_Error();
        return;
    }
    else
    {
        //Vraceni hodnoty prvniho prvku
        *dataPtr = list->firstElement->data;
    }
}

/**
 * Prostřednictvím parametru dataPtr vrátí hodnotu posledního prvku seznamu list.
 * Pokud je seznam list prázdný, volá funkci DLL_Error().
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param dataPtr Ukazatel na cílovou proměnnou
 */
void DLL_GetLast( DLList *list, int *dataPtr ) {

        //Osetreni neprazdnosti seznamu
    if(list->firstElement == NULL)
    {
        DLL_Error();
        return;
    }
    else
    {
        //Vraceni hodnoty poseldniho prvku
        *dataPtr = list->lastElement->data;
    }
}

/**
 * Zruší první prvek seznamu list.
 * Pokud byl první prvek aktivní, aktivita se ztrácí.
 * Pokud byl seznam list prázdný, nic se neděje.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_DeleteFirst( DLList *list ) {

    //Osetreni neprazdnosti
    if(list->firstElement != NULL)
    {
        //Ziskam si pomocnym pointerem prvni element v listu 
        DLLElementPtr tmp = list->firstElement;

        //Osetreni aktivniho prvku
        if(list->activeElement == list->firstElement)
        {
            list->activeElement = NULL;
        }

        //Pro jedno prvkovy list
        if(list->firstElement->nextElement == NULL)
        {
            //V tomto pripade je jeden prvek prvni i posledni, takze chceme smazat oba
            list->lastElement = NULL;
            list->firstElement = NULL;
            free(tmp);
        }
        else
        {
            //Mazeme prvni, takze druhy musi mit previous ukazatel na NULL
            tmp->nextElement->previousElement = NULL;
            //Nastavim novy prvni prvek, coz je prvek za mym novym prvkem
            list->firstElement = tmp->nextElement;
            //uvolnim pamet
            free(tmp);
        }
    }
}

/**
 * Zruší poslední prvek seznamu list.
 * Pokud byl poslední prvek aktivní, aktivita seznamu se ztrácí.
 * Pokud byl seznam list prázdný, nic se neděje.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_DeleteLast( DLList *list ) {

    //Osetreni neprazdnosti
    if(list->firstElement != NULL)
    {
        //Ziskam si pomocnym pointerem posledni element v listu 
        DLLElementPtr tmp = list->lastElement;

        //Osetreni aktivniho prvku
        if(list->activeElement == list->lastElement)
        {
            list->activeElement = NULL;
        }

        //pro jedno prvkovy list
        if(list->firstElement->nextElement == NULL)
        {
            //V tomto pripade je jeden prvek prvni i posledni, takze chceme smazat oba
            list->firstElement = NULL;
            free(tmp);
        }
        else
        {
            //Mazeme posledni, takze predposledni musi mit next ukazatel na NULL
            tmp->previousElement->nextElement = NULL;
            //Nastavim novy posledni prvek listu
            list->lastElement = tmp->previousElement;
            //Uvolnim pamet
            free(tmp);
        }
    }

}

/**
 * Zruší prvek seznamu list za aktivním prvkem.
 * Pokud je seznam list neaktivní nebo pokud je aktivní prvek
 * posledním prvkem seznamu, nic se neděje.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_DeleteAfter( DLList *list ) {

    //SPlneni podminky, ze mame aktivni prvek a zaroven aktivnim prvkem neni posledni prvek
    if(list->activeElement != NULL && list->activeElement != list->lastElement)
    {
        DLLElementPtr tmp = list->activeElement->nextElement;

        //Osetreni situace, kdy mazany prvek je poslednim prvkem v seznamu
        if(tmp == list->lastElement)
        {
            list->activeElement->nextElement = NULL;
            list->lastElement = list->activeElement;
            tmp->previousElement = NULL;
            free(tmp);
        }
        else
        {
            list->activeElement->nextElement = tmp->nextElement;
            tmp->nextElement->previousElement = list->activeElement;
            free(tmp);
        }
        
    }

}

/**
 * Zruší prvek před aktivním prvkem seznamu list .
 * Pokud je seznam list neaktivní nebo pokud je aktivní prvek
 * prvním prvkem seznamu, nic se neděje.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_DeleteBefore( DLList *list ) {

    //Splneni podminek ze mame aktivni prvek a zaroven aktivnim prvkem neni prvni prvek
    if(list->activeElement != NULL && list->activeElement != list->firstElement)
    {
        DLLElementPtr tmp = list->activeElement->previousElement;

        //osetreni situace, kdy mazany prvek je prvnim prvkem v seznamu
        if(tmp == list->firstElement)
        {
            list->activeElement->previousElement = NULL;
            list->firstElement = list->activeElement;
            tmp->nextElement = NULL;
            free(tmp);
        }
        else
        {
            list->activeElement->previousElement = tmp->previousElement;
            tmp->previousElement->nextElement = list->activeElement;
            free(tmp);
        }
    }
}

/**
 * Vloží prvek za aktivní prvek seznamu list.
 * Pokud nebyl seznam list aktivní, nic se neděje.
 * V případě, že není dostatek paměti pro nový prvek při operaci malloc,
 * volá funkci DLL_Error().
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param data Hodnota k vložení do seznamu za právě aktivní prvek
 */
void DLL_InsertAfter( DLList *list, int data ) {

    //Splneni podminky, ze mame aktivni prvek
    if(list->activeElement != NULL)
    {
        DLLElementPtr tmp = malloc(sizeof(struct DLLElement));
        //Osetreni toho, ze mame dost pameti
        if(tmp == NULL)
        {
            DLL_Error();
            return;
        }

        //predam data prvku
        tmp->data = data;
        //Dozadu bude ukazovat na active element
        tmp->previousElement = list->activeElement;

        //Osetreni pripadu, kdy aktivni element je poslednim elementem
        if(list->activeElement == list->lastElement)
        {
            //Aktivni element bude dopredu ukazovat na tmp
            list->activeElement->nextElement = tmp;
            //Nyni je tmp poslednim prvkem seznamu, takze dopredu ukazuje na NULL
            tmp->nextElement = NULL;
            //Pointer lastElement ukazuje na tmp
            list->lastElement = tmp;
        }
        else
        {
            //dopredu bude ukazovat na element za aktivnim elementem
            tmp->nextElement = list->activeElement->nextElement;
            //Prvek co za sebe ukazoval na aktivni element bude dozadu ukazovat na novy element
            tmp->nextElement->previousElement = tmp;
            //Aktivnim elementem ukazu dopredu na novy element
            list->activeElement->nextElement = tmp;
        }
    }
}

/**
 * Vloží prvek před aktivní prvek seznamu list.
 * Pokud nebyl seznam list aktivní, nic se neděje.
 * V případě, že není dostatek paměti pro nový prvek při operaci malloc,
 * volá funkci DLL_Error().
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param data Hodnota k vložení do seznamu před právě aktivní prvek
 */
void DLL_InsertBefore( DLList *list, int data ) {

    //Splneni podminky, ze mame aktivni prvek
    if(list->activeElement != NULL)
    {
        DLLElementPtr tmp = malloc(sizeof(struct DLLElement));
        //Osetreni toho, ze mame dost pameti
        if(tmp == NULL)
        {
            DLL_Error();
            return;
        }

        //predam data prvku
        tmp->data = data;
        //dopredu bude ukazovat na active element
        tmp->nextElement = list->activeElement;

        //Osetreni pripadu, kdy aktivni element je prvnim elementem
        if(list->activeElement == list->firstElement)
        {
            //Aktivnim elementem ukazujeme dozadu na tmp
            list->activeElement->previousElement = tmp;
            //tmp je poslednim prvkem, takze dozadu ukazuje na NULL
            tmp->previousElement = NULL;
            //Pointer firstElement ukazuje na tmp
            list->firstElement = tmp;
        }
        else
        {
            //Prvek co byl za aktivnim elementem ukazuje dopredu na tmp
            list->activeElement->previousElement->nextElement = tmp;
            //Tmp ukazuje dozadu na prvek, ktery byl za aktivnim prvke
            tmp->previousElement = list->activeElement->previousElement;
            //prvek co byl za aktivnim elementem ukazuje dopredu na tmp
            list->activeElement->previousElement->nextElement = tmp;

        }
    }
}

/**
 * Prostřednictvím parametru dataPtr vrátí hodnotu aktivního prvku seznamu list.
 * Pokud seznam list není aktivní, volá funkci DLL_Error ().
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param dataPtr Ukazatel na cílovou proměnnou
 */
void DLL_GetValue( DLList *list, int *dataPtr ) {

    if(list->activeElement == NULL)
    {
        DLL_Error();
        return;
    }
    else
    {
        *dataPtr = list->activeElement->data;
    }
}

/**
 * Přepíše obsah aktivního prvku seznamu list.
 * Pokud seznam list není aktivní, nedělá nic.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param data Nová hodnota právě aktivního prvku
 */
void DLL_SetValue( DLList *list, int data ) {

    //Pokud je aktivita neprazdna, prepisu data
    if(list->activeElement != NULL)
    {
        list->activeElement->data = data;
    }

}

/**
 * Posune aktivitu na následující prvek seznamu list.
 * Není-li seznam aktivní, nedělá nic.
 * Všimněte si, že při aktivitě na posledním prvku se seznam stane neaktivním.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_Next( DLList *list ) {

    //Pokud je aktivita neprazdna, posunu ji dopredu
    if(list->activeElement != NULL)
    {
        list->activeElement = list->activeElement->nextElement;
    }
}


/**
 * Posune aktivitu na předchozí prvek seznamu list.
 * Není-li seznam aktivní, nedělá nic.
 * Všimněte si, že při aktivitě na prvním prvku se seznam stane neaktivním.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_Previous( DLList *list ) {

    if(list->activeElement != NULL)
    {
        list->activeElement = list->activeElement->previousElement;
    }
}

/**
 * Je-li seznam list aktivní, vrací nenulovou hodnotu, jinak vrací 0.
 * Funkci je vhodné implementovat jedním příkazem return.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 *
 * @returns Nenulovou hodnotu v případě aktivity prvku seznamu, jinak nulu
 */
int DLL_IsActive( DLList *list ) {

    //Ukazatel na aktivni element neni NULL
    return list->activeElement != NULL;
}

/* Konec c206.c */
