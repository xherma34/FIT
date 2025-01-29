
/* ******************************* c204.c *********************************** */
/*  Předmět: Algoritmy (IAL) - FIT VUT v Brně                                 */
/*  Úkol: c204 - Převod infixového výrazu na postfixový (s využitím c202)     */
/*  Referenční implementace: Petr Přikryl, listopad 1994                      */
/*  Přepis do jazyka C: Lukáš Maršík, prosinec 2012                           */
/*  Upravil: Kamil Jeřábek, září 2019                                         */
/*           Daniel Dolejška, září 2021                                       */
/* ************************************************************************** */
/*
** Implementujte proceduru pro převod infixového zápisu matematického výrazu
** do postfixového tvaru. Pro převod využijte zásobník (Stack), který byl
** implementován v rámci příkladu c202. Bez správného vyřešení příkladu c202
** se o řešení tohoto příkladu nepokoušejte.
**
** Implementujte následující funkci:
**
**    infix2postfix ... konverzní funkce pro převod infixového výrazu
**                      na postfixový
**
** Pro lepší přehlednost kódu implementujte následující pomocné funkce:
**    
**    untilLeftPar ... vyprázdnění zásobníku až po levou závorku
**    doOperation .... zpracování operátoru konvertovaného výrazu
**
** Své řešení účelně komentujte.
**
** Terminologická poznámka: Jazyk C nepoužívá pojem procedura.
** Proto zde používáme pojem funkce i pro operace, které by byly
** v algoritmickém jazyce Pascalovského typu implemenovány jako procedury
** (v jazyce C procedurám odpovídají funkce vracející typ void).
**
**/

#include "c204.h"

int solved;

/**
 * Pomocná funkce untilLeftPar.
 * Slouží k vyprázdnění zásobníku až po levou závorku, přičemž levá závorka bude
 * také odstraněna.
 * Pokud je zásobník prázdný, provádění funkce se ukončí.
 *
 * Operátory odstraňované ze zásobníku postupně vkládejte do výstupního pole
 * znaků postfixExpression.
 * Délka převedeného výrazu a též ukazatel na první volné místo, na které se má
 * zapisovat, představuje parametr postfixExpressionLength.
 *
 * Aby se minimalizoval počet přístupů ke struktuře zásobníku, můžete zde
 * nadeklarovat a používat pomocnou proměnnou typu char.
 *
 * @param stack Ukazatel na inicializovanou strukturu zásobníku
 * @param postfixExpression Znakový řetězec obsahující výsledný postfixový výraz
 * @param postfixExpressionLength Ukazatel na aktuální délku výsledného postfixového výrazu
 */
void untilLeftPar( Stack *stack, char *postfixExpression, unsigned *postfixExpressionLength ) {

    //Promenna pro ulozeni vrcholu zasobniku
    char topChar;

    //Pruchod zasobnikem, dokud neni prazdny a dokud nenarazi na ()
    while(!Stack_IsEmpty(stack))
    {
        //Nactu si vrchol zasobniku do promenne
        Stack_Top(stack, &topChar);
        //Popnu nacteny prvek ze zasobniku
        Stack_Pop(stack);
        //Pokud je na vrcholu zasobniku jiz nacetla (
        if(topChar == '(')
        {
            break;
        }
        //Zapisu vybrany prvek do retezce
        postfixExpression[*postfixExpressionLength] = topChar;
        //Inkrementuji promennou pro pruchod retezcem
        (*postfixExpressionLength)++;
    }
}

/**
 * Pomocná funkce doOperation.
 * Zpracuje operátor, který je předán parametrem c po načtení znaku ze
 * vstupního pole znaků.
 *
 * Dle priority předaného operátoru a případně priority operátoru na vrcholu
 * zásobníku rozhodneme o dalším postupu.
 * Délka převedeného výrazu a taktéž ukazatel na první volné místo, do kterého
 * se má zapisovat, představuje parametr postfixExpressionLength, výstupním
 * polem znaků je opět postfixExpression.
 *
 * @param stack Ukazatel na inicializovanou strukturu zásobníku
 * @param c Znak operátoru ve výrazu
 * @param postfixExpression Znakový řetězec obsahující výsledný postfixový výraz
 * @param postfixExpressionLength Ukazatel na aktuální délku výsledného postfixového výrazu
 */
void doOperation( Stack *stack, char c, char *postfixExpression, unsigned *postfixExpressionLength ) {

    //Promenna pro ulozeni vrcholu zasobniku
    char topChar;

    //Osetreni podminek viz. prezentace predmetu IAL
    //Osetreni prazdneho zasobniku
    if(Stack_IsEmpty(stack))
    {
        Stack_Push(stack, c);
        return;
    }
    Stack_Top(stack, &topChar);

    //Pokud se na vrcholu zasobniku vyskytuje (
    if(topChar == '(')
    {
        Stack_Push(stack, c);
        return;
    }

    //Pokud je na vrcholu zasobniku operator s nizsi prioritou
    if(topChar == '+' || topChar == '-')
    {
        //Pokud vkladam operator s vyssi prioritou
        if(c == '*' || c == '/')
        {
            Stack_Push(stack, c);
            return;
        }
    }

    //Kdyz umistim operator na vrchol, musim si ho ulozit do retezcce postfixExpression
    postfixExpression[*postfixExpressionLength] = topChar;
    //Inkrementuji promennou pro pruchod retezcem
    (*postfixExpressionLength)++;
    //Vyhodim zpracovany prvek
    Stack_Pop(stack);

    //Pokud nesplnim ani jednu podminku, rekurzivne zavolam dooPeration
    doOperation(stack, c, postfixExpression, postfixExpressionLength);

}

/**
 * Konverzní funkce infix2postfix.
 * Čte infixový výraz ze vstupního řetězce infixExpression a generuje
 * odpovídající postfixový výraz do výstupního řetězce (postup převodu hledejte
 * v přednáškách nebo ve studijní opoře).
 * Paměť pro výstupní řetězec (o velikosti MAX_LEN) je třeba alokovat. Volající
 * funkce, tedy příjemce konvertovaného řetězce, zajistí korektní uvolnění zde
 * alokované paměti.
 *
 * Tvar výrazu:
 * 1. Výraz obsahuje operátory + - * / ve významu sčítání, odčítání,
 *    násobení a dělení. Sčítání má stejnou prioritu jako odčítání,
 *    násobení má stejnou prioritu jako dělení. Priorita násobení je
 *    větší než priorita sčítání. Všechny operátory jsou binární
 *    (neuvažujte unární mínus).
 *
 * 2. Hodnoty ve výrazu jsou reprezentovány jednoznakovými identifikátory
 *    a číslicemi - 0..9, a..z, A..Z (velikost písmen se rozlišuje).
 *
 * 3. Ve výrazu může být použit předem neurčený počet dvojic kulatých
 *    závorek. Uvažujte, že vstupní výraz je zapsán správně (neošetřujte
 *    chybné zadání výrazu).
 *
 * 4. Každý korektně zapsaný výraz (infixový i postfixový) musí být uzavřen
 *    ukončovacím znakem '='.
 *
 * 5. Při stejné prioritě operátorů se výraz vyhodnocuje zleva doprava.
 *
 * Poznámky k implementaci
 * -----------------------
 * Jako zásobník použijte zásobník znaků Stack implementovaný v příkladu c202.
 * Pro práci se zásobníkem pak používejte výhradně operace z jeho rozhraní.
 *
 * Při implementaci využijte pomocné funkce untilLeftPar a doOperation.
 *
 * Řetězcem (infixového a postfixového výrazu) je zde myšleno pole znaků typu
 * char, jenž je korektně ukončeno nulovým znakem dle zvyklostí jazyka C.
 *
 * Na vstupu očekávejte pouze korektně zapsané a ukončené výrazy. Jejich délka
 * nepřesáhne MAX_LEN-1 (MAX_LEN i s nulovým znakem) a tedy i výsledný výraz
 * by se měl vejít do alokovaného pole. Po alokaci dynamické paměti si vždycky
 * ověřte, že se alokace skutečně zdrařila. V případě chyby alokace vraťte namísto
 * řetězce konstantu NULL.
 *
 * @param infixExpression Znakový řetězec obsahující infixový výraz k převedení
 *
 * @returns Znakový řetězec obsahující výsledný postfixový výraz
 */
char *infix2postfix( const char *infixExpression ) {

    //Promenna pro indexovani retezce
    unsigned int postfixLen = 0;

    char currChar = *infixExpression;

    //Inicializace retezce
    char *postfixExpression = malloc (MAX_LEN * sizeof(char));
    //Inicializace zasobniku
    Stack *stack = malloc(sizeof(Stack));
    Stack_Init(stack);


    while(currChar != '\0')
    {
        //Zpracovani podminek viz zadani
        //Zpracovani leve zavorky
        if(currChar == '(')
        {
            Stack_Push(stack, currChar);
        }
        //ZPracovani operatoru
        else if(currChar == '+' || currChar == '-' || currChar == '*' || currChar == '/')
        {
            doOperation(stack, currChar, postfixExpression, &postfixLen);
        }
        //Zpracovani prave zavorky
        else if(currChar == ')')
        {
            untilLeftPar(stack, postfixExpression, &postfixLen);
        }
        //Zpracovani operandu
        else if((currChar >= '0' && currChar <= '9') || (currChar >= 'a' && currChar <= 'z') || (currChar >= 'A' && currChar <= 'Z'))
        {
            postfixExpression[postfixLen] = currChar;
            postfixLen++;
        }
        else if(currChar == '=')
        {
            //Vyprazdnim si zasobnik, protoze jsem na konci
            while(!Stack_IsEmpty(stack))
            {
                //Vyberu si vrchol zasobniku
                Stack_Top(stack, &postfixExpression[postfixLen]);
                //Inkrementuji
                postfixLen++;
                //Popnu stack
                Stack_Pop(stack);
            }
            //Narvu na konec retezce =
            postfixExpression[postfixLen] = '=';
            postfixLen++;
            //Osetrim ukonceni zpracovani retezce
            postfixExpression[postfixLen] = '\0';
        }
        
        currChar = *(++infixExpression);
    }
    free(stack);
    return postfixExpression;
}

/* Konec c204.c */
