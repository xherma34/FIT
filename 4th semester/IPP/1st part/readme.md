## Implementační dokumentace k 1. úloze do IPP 2021/2022
#### Jméno a příjmení: Pavel Heřmann
#### Login: xherma34

### Zadání projektu
Za úkol bylo implementovat skript parse.php, který překládá program v jazyce IPPcode22 do XML

### Spuštění programu
Program se spouští příkazem: <br>**php8.1 parse.php** - v tomto případě se vstup načítá ze standartního vstupu a vypisuje
na standartní výstup.<br>
Lze načítat ze vstupního souboru a zároveň zapisovat do výstupního souboru, poté se program spouští ve tvaru:<br>
**php8.1 parse.php < IFILE > OFILE** <br>
Jelikož program nepodporuje žádná další rozšíření, lze spustit program pouze s argumentem --help a to ve tvaru: <br>
**php8.1 parse.php --help**

### Fungování programu
Hlavní část programu tvoří while cyklus, který se stará o dvě části: zpracování vstupního
souboru a úprava vstupu, kontrola správnosti instrukcí a následné přetvoření programu v
jazyce IPPcode22 na formát XML.

#### Zpracování vstupu
Ze vstupu se načte řádek, který se zpracuje instrukcí **trim**, která z řádku odstřihne
bílé znaky. Následně se ostříhaný string rozkouskuje na části (jednu část tvoří
souvislý text po první mezeru) a uloží do proměnné, která má jednotlivé části řádku rozdělené
po indexech. <br>
V této části se zároveň kontrolují komentáře, při detekci komentáře se buď odstřihne část
komentáře (pokud je za instrukcí), nebo se odstřihne celý řádek.

#### Kontrola instrukcí a vytváření XML
První krok při kontrole je načtení hlavičky souboru. Každý kód IPPcode22 musí začínat
instrukcí ".IPPcode22", pokud program již přeskočil prázdné řádky a vymazal všechny komentáře, ale
hlavičku stále nenašel, tak dojde k chybě 21, pokud program hlavičku našel, tak nastaví
hodnotu proměnné "headerBool" na true, vypíše hlavičku XML souboru a zpracovanou hlavičkovou instrukci IPPcode22.
Následně program začne zpracovávat instrukce. K tomu slouží pomocné funkce na rozlišení proměnné, 
návšstí a konstanty, pokud je u instrukce špatný počet argumentů, nebo jsou
špatného typu, dojde k chybě 22. <br> V opačném případě dochází k tisknutí na standartní
výstp pomocí instrukce echo. Při tisknutí jsou také využívány pomocné funkce a to hlavně pro
argumenty typu symb. **SymbType** získává z argumentu konstanty typ a **CutValue** vystřihuje
hodnotu, která má být vytisknuta.

