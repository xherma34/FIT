## Implementační dokumentace k 2. úloze do IPP 2021/2022
#### Jméno a příjmení: Pavel Heřmann
#### Login: xherma34

### Zadání projektu
Za úkol bylo implementovat skript interpret.py, který interpretuje xml soubor vytvořený 
v předchozí části projektu.

### Spuštění skriptu
Skript se spouští příkazem `$ interpret.py --ARGUMENT=FILE [OPT]--ARGUMENT=FILE` <br>
Argumentem lze skriptu dát zdrojový soubor xml nebo vstupní soubor pro 
čtení instrukce *READ*. Pro spuštění skriptu je *nutno zadat alespoň jeden z 
těchto argumentů*, soubor který není zadán argumentem se načítá ze *standartního vstupu*.
Pokud není zadán ani jeden z argumentů, skript vyhodí chybu. <br>
`--source=SFILE` kde SFILE je název zdrojového xml souboru <br>
`--input=IFILE` ked IFILE je název vstupního souboru pro instrukci *READ*

### Fungování skritpu
#### Hlavní tělo skritpu
Při spuštění skriptu je vytvořen globální seznam *labels* a rámec *programFrame*, tento rámec
funguje v skriptu jako globální rámec ostatních rámců. Následně je volána funkce `main()`, 
která pomocí knihovny argparse načítá ze vstupu zdrojový a vstupní soubor, oba soubory jsou poté
uloženy do proměnné a zasílány do funkce `checkFiles()`, která se podívá do aktuálního adresáře,
jestli dané jméno souboru existuje. Dále se ve funkci main vytvoří instance třídy `Interpreter`, poté se invokuje její metoda `parseTree`. <br>
Zde se vytvoří xml strom pomocí knihovny `xml.etree.ElementTree`, zároveň se program pokusí otevřít vstupní soubor do třídní proměnné `ReadFrom`. Poté se v metodě začnou načítat isntrukce ze stromu a dochází ke kontrole formátu XML, každá instrukce je inicializována jako instance třídy `Instruction`, při inicializaci se kontroluje její formát.
Instrukce jsou po inicializaci ukládány do pole nad kterým se provede funkce `sort`, která je seřadí. V tomto seznamu následně najde všechny návěští a uloží je do globálního pole `labels`. <br>
Jednotlivé instrukce invokují metodu `execute`, zde dochází k inkrementaci třídní proměnné
`count`, která drží informaci o počtu instrukcí a pomáhá poté při provádění skoků. V této metodě se podle názvu instrukce vybere, co se bude s instrukcí dále dělat. <br>

#### Práce s rámci
Pro práci s rámci byla vytvořena již zmíněná instance třídy `Frame`. Při plnění instrukcí v hlavním těle programu jsou volány třídní metody různých tříd. Všechny rámce (globální, lokální, dočasný) jsou třídní proměnnou třídy `Frame`, aby se s hodnotami ukládanými do rámců pracovalo lépe, tak byla vytvořena třída `FrameAtom`. <br>
Při vkládání do rámce je zaslán do metody celý název proměnné. Voláním ostatních metod se získá rámec a název, následně se vytvoří instance třídy `FrameAtom`, která má zatím pouze název. Ta je vložena do příslušného rámce. <br>
Pro zjednodušení práce s proměnnými v rámcích jsou dále implementovány metody, které např.: zjistí hodnotu a typ proměnné podle jména, aktualizují hodnotu a typ proměnné podle jména atd. Při jakékoliv práci s proměnnou v ramcí je provedena kontrola zda daná proměnná existuje v daném rámci.

### Rozšíření NVP
V projektu jsem použil třídy: Label, Frame, FrameAtom, Instruction, Interpreter. Hlavní motivací bylo si zkusit OOP v praxi a zároveň to bylo pohodlnější pro implementaci.