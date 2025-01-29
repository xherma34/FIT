<?php

/**
 * @author Pavel Heřmann
 * IPP parse.php 2022 - 1. část
 **/

    //Vraci hodnotu 1 pokud splnuje string zadani <var>
    function IsVariable($string)
    {
        if(preg_match("/^(LF|TF|GF)@[a-zA-Z_$&%*!?-][a-zA-Z0-9_$&%*!?-]*$/", $string))
        {
            return 1;
        }
    }

    //Vraci hodnotu 1 pokud splnuje string zadani <label>
    function IsLabel($string)
    {
        if(preg_match("/^[a-zA-Z_$&%*!?-][a-zA-Z0-9_$&%*!?-]+$/", $string))
        {
            return 1;
        }
    }

    //Vraci hodnotu 1 pokud splnuje string zadani Type
    function IsType($string)
    {
        if(strcmp($string, "string") || strcmp($string, "int") || strcmp($string, "bool"))
        {
            return 1;
        }
        return 0;
    }

    //Vraci hodnotu 1 pokud splnuje zadani <symb>
    function IsSymbol($string)
    {
        //Pokud se jedna o var -> taktez muze byt symb
        if(IsVariable($string))
        {
            return 1;
        }
        //Musi mit tvar bool@true nebo bool@false
        else if(preg_match("/^bool@(true|false)$/", $string))
        {
            return 1;
        }
        //Musi mit tvar int@(volitelne + a -)[Rozhrani cisel 0-9](+ - pozitivni iterace) nebo int@
        else if(preg_match("/^int@([+|-]?[0-9]+|)$/", $string))
        {
            return 1;
        }
        //Musi mit tvar nil@nil nebo nil@
        else if(preg_match("/^(nil@nil|nil@)$/", $string))
        {
            return 1;
        }
        //TODO proc nejedou dve lomitka
        //Musi mit tvar string@[nesmi obsahovat samotny \] nebo \035 nebo \092 nebo \0[0,1,2][0-9] nebo \03[0,1,2]
        //Tento cely tvar iterovany aby slo zadat prazdny string -> string@
        else if(preg_match("/^string@([^\\\\]|\\\\035|\\\\092|\\\\0[0-2][0-9]|\\\\03[0-2])*$/", $string))
        {
            return 1;
        }
        return 0;
    }

    //Pomocna funkce pro print, vraci typ <symb>
    function SymbType($string)
    {
        if(IsVariable($string))
        {
            return "var";
        }
        else if(preg_match("/^bool@/", $string))
        {
            return "bool";
        }
        else if(preg_match("/^nil@/", $string))
        {
            return "nil";
        }
        else if(preg_match("/^int@/", $string))
        {
            return "int";
        }
        else if(preg_match("/^string@/", $string))
        {
            return "string";
        }
    }

    //Vystrihne hodnotu pripisovanou do dat. typu, pokud se nejedna o var
    //pr.: int@12 vraci--> 12
    function CutValue($string)
    {
        if(IsVariable($string)) return $string;
        $len = strpos($string, "@");
        $string = substr($string, $len+1);
        return $string;
    }

    //Funkce tiskne na stderr hodnotu prislusnou dane chybe
    function ErrorExit($value)
    {
        exit($value);
    }

    /**********************************************MAIN BODY****************************************************/
    //Errorove vystupy
    $ErrParam = 10;     //Spatne zadam argumenty pri spusteni
    $ErrInputFile = 11;     //Chyba pri nacitani InputFile
    $ErrOutputFile = 12;        //Chyba pri zapisovani na outputFile
    $ErrIntern = 99;        //Interni chyba programu
    $ErrNoHeader = 21;      //Chybejici hlavicak
    $ErrOpCode = 22;        //Zadani funkce, ktera v IPPcode22 neexistuje
    $ErrLexSynt = 23;       //Ostatni syntakticke/lexykalni chyby

    //setup
    ini_set('display_errors', 'stderr');

    //promenne
    $headerBool = false;

    if($argc == 2)
    {
        if($argv[1] == "--help")
        {
            echo "Program nepodpořuje žádná rozšíření, jediný možný vstupní argument je --help\n";
            echo "Program lze spouštět v následujícím formátu:\n";
            echo "php8.1 parse.php < IFILE\n kde IFILE je vámi zvolený vstupní soubor\n";
            echo "Lze printovat výstup do souboru následovně:\n";
            echo "php8.1 parse.php < IFILE > OFILE\n";
            echo "Kde OFILE je výstupní soubor xml";
            exit(0);
        }
    }
    else if($argc != 1)
    {
        exit($ErrParam);
    }

    $orderCount = 0;
    while($line = fgets(STDIN))
    {
        //Trim kdyby nastalo na zacatku radku:" #KOMENT"
        $line = trim($line);

        //Mazani komentare
        if(str_contains($line, "#"))
        {
            $line = substr($line, 0, strpos($line, "#"));
            $line = trim($line);
        }

        //Pokud je prazdny line --> skip
        if($line == "")
        {
            continue;
        }

        //Rozrezani stringu na array
        $cutLine = explode(' ', trim($line, ));

        //Load hlavicky souboru
        if(!$headerBool)
        {
            if($cutLine[0] == ".IPPcode22")
            {
                $headerBool = true;
                //XML header
                echo "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"."\n";
                //Head
                echo "<program language=\"IPPcode22\">"."\n";
                $orderCount++;
                continue;
            }
            else
            {
                ErrorExit($ErrNoHeader);
            }
        }

        //Pokud jsme nacetli hlavicku
        if($headerBool)
        {
            //TODO predelat "\t" na 4 mezery
            switch (strtoupper($cutLine[0]))
            {
                //ARGUMENT: 1. VAR
                //          2. SYMB
                //DELKA: 3
                case "MOVE":
                case "INT2CHAR":
                case "STRLEN":
                case "TYPE":
                    if(count($cutLine) == 3)
                    {
                        if(IsVariable($cutLine[1]) && IsSymbol($cutLine[2]))
                        {
                            echo "    "."<instruction order="."\"".$orderCount."\" "."opcode="."\"".$cutLine[0]."\">"."\n";
                            echo "    "."    "."<arg1 type="."\""."var"."\">".$cutLine[1]."</arg1>"."\n";
                            echo "    "."    "."<arg2 type="."\"".SymbType($cutLine[2])."\">".CutValue($cutLine[2])."</arg2>"."\n";
                            echo "    "."</instruction>"."\n";
                        }
                        else
                        {
                            ErrorExit($ErrLexSynt);
                        }
                    }
                    else
                    {
                        ErrorExit($ErrLexSynt);
                    }
                break;

                //BEZ-ARGUMENTOVE
                //DELKA: 1
                case "CREATEFRAME":
                case "PUSHFRAME":
                case "RETURN":
                case "POPFRAME":
                case "BREAK";
                    if(count($cutLine) == 1)
                    {
                        echo "    "."<instruction order="."\"".$orderCount."\" "."opcode="."\"".$cutLine[0]."\">"."\n";
                        echo "    "."</instruction>"."\n";
                    }
                    //TODO zeptat se
                    else
                    {
                        ErrorExit($ErrLexSynt);
                    }
                break;

                //ARGUMENT: 1. VAR
                //DELKA: 2
                case "DEFVAR":
                case "POPS":
                    if(count($cutLine) == 2)
                    {
                        if(IsVariable($cutLine[1]))
                        {
                            echo "    "."<instruction order="."\"".$orderCount."\" "."opcode="."\"".$cutLine[0]."\">"."\n";
                            echo "    "."    "."<arg1 type="."\""."var"."\">".$cutLine[1]."</arg1>"."\n";
                            echo "    "."</instruction>"."\n";
                        }
                        else
                        {
                            ErrorExit($ErrLexSynt);
                        }
                    }
                    else
                    {
                        ErrorExit($ErrLexSynt);
                    }
                break;

                //ARGUMENT: 1. LABEL
                //DELKA: 2
                case "CALL":
                case "LABEL":
                case "JUMP":
                     if(count($cutLine) == 2)
                     {
                         if(IsLabel($cutLine[1]))
                         {
                             echo "    "."<instruction order="."\"".$orderCount."\" "."opcode="."\"".$cutLine[0]."\">"."\n";
                             echo "    "."    "."<arg1 type="."\""."label"."\">".$cutLine[1]."</arg1>"."\n";
                             echo "    "."</instruction>"."\n";
                         }
                         else
                         {
                             ErrorExit($ErrLexSynt);
                         }
                     }
                     else
                     {
                         ErrorExit($ErrLexSynt);
                     }
                 break;

                //ARGUMENT: 1. SYMB
                //DELKA: 2
                case "PUSHS":
                case "WRITE":
                case "EXIT":
                case "DPRINT":
                    if(count($cutLine) == 2)
                    {
                        if(IsSymbol($cutLine[1]))
                        {
                            echo "    "."<instruction order="."\"".$orderCount."\" "."opcode="."\"".$cutLine[0]."\">"."\n";
                            echo "    "."    "."<arg1 type="."\"".SymbType($cutLine[1])."\">".CutValue($cutLine[1])."</arg1>"."\n";
                            echo "    "."</instruction>"."\n";
                        }
                        else
                        {
                            ErrorExit($ErrLexSynt);
                        }
                    }
                    else
                    {
                        ErrorExit($ErrLexSynt);
                    }
                break;

                //ARGUMENT: 1. VAR
                //          2. SYMB
                //          3. SYMB
                //DELKA: 4
                case "ADD":
                case "SUB":
                case "MUL":
                case "IDIV":
                case "LT":
                case "GT":
                case "EQ":
                case "AND":
                case "OR":
                case "NOT":
                case "STRI2INT":
                case "CONCAT":
                case "GETCHAR":
                case "SETCHAR":
                    if(count($cutLine) == 4)
                    {
                        if(IsVariable($cutLine[1]) && IsSymbol($cutLine[2]) && IsSymbol($cutLine[3]))
                        {
                            echo "    "."<instruction order="."\"".$orderCount."\" "."opcode="."\"".$cutLine[0]."\">"."\n";
                            echo "    "."    "."<arg1 type="."\""."var"."\">".$cutLine[1]."</arg1>"."\n";
                            echo "    "."    "."<arg2 type="."\"".SymbType($cutLine[2])."\">".CutValue($cutLine[2])."</arg2>"."\n";
                            echo "    "."    "."<arg3 type="."\"".SymbType($cutLine[3])."\">".CutValue($cutLine[3])."</arg3>"."\n";
                            echo "    "."</instruction>"."\n";
                        }
                        else
                        {
                            ErrorExit($ErrLexSynt);
                        }
                    }
                    else
                    {
                        ErrorExit($ErrLexSynt);
                    }
                break;

                //ARGUMENT: 1. VAR
                //          2. TYPE
                //DELKA: 3
                case "READ":
                    if(count($cutLine) == 3)
                    {
                        if(IsVariable($cutLine[1]) && IsType($cutLine[2]))
                        {
                            echo "    "."<instruction order="."\"".$orderCount."\" "."opcode="."\"".$cutLine[0]."\">"."\n";
                            echo "    "."    "."<arg1 type="."\""."var"."\">".$cutLine[1]."</arg1>"."\n";
                            echo "    "."    "."<arg2 type="."\""."type"."\">".$cutLine[2]."</arg2>"."\n";
                            echo "    "."</instruction>"."\n";
                        }
                        else
                        {
                            ErrorExit($ErrLexSynt);
                        }
                    }
                    else
                    {
                        ErrorExit($ErrLexSynt);
                    }
                break;

                //ARGUMENT: 1. LABEL
                //          2. SYMB
                //          3. SYMB
                //DELKA: 4
                case "JUMPIFEQ":
                case "JUMPIFNEQ":
                if(count($cutLine) == 4)
                {
                    if(IsLabel($cutLine[1]) && IsSymbol($cutLine[2]) && IsSymbol($cutLine[3]))
                    {
                        echo "    "."<instruction order="."\"".$orderCount."\" "."opcode="."\"".$cutLine[0]."\">"."\n";
                        echo "    "."    "."<arg1 type="."\""."label"."\">".$cutLine[1]."</arg1>"."\n";
                        echo "    "."    "."<arg2 type="."\"".SymbType($cutLine[2])."\">".CutValue($cutLine[2])."</arg2>"."\n";
                        echo "    "."    "."<arg3 type="."\"".SymbType($cutLine[3])."\">".CutValue($cutLine[3])."</arg3>"."\n";
                        echo "    "."</instruction>"."\n";
                    }
                    else
                    {
                        ErrorExit($ErrLexSynt);
                    }
                }
                else
                {
                    ErrorExit($ErrLexSynt);
                }
                break;

                default:
                    ErrorExit($ErrOpCode);
                break;
            }
            //pricitani poctu instrukci pro print
            $orderCount++;
        }
    }
    echo "</program>";
?>