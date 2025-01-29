import sys
import re
import argparse
import xml.etree.ElementTree as ET
from sys import stdin
import os.path
#TODO projet vsechny exit(1) a dat jim spravnou hodnotu

class Interpreter:
    count = 0
    ReadFrom = ""

    def __init__(self, sourceFile, inputFile):
        self.source = sourceFile
        self.input = inputFile

    def parseTree(self):
        # Zkouska ulozeni xml do stromu
        if self.source != stdin:
            try:
                tree = ET.parse(self.source[0])
            except:
                exit(31)
        else:
            tree = ET.parse(stdin)
        root = tree.getroot()

        # zkouska otevreni input souboru
        try:
            Interpreter.ReadFrom = open(self.input[0], "r")
        except:
            exit(11)

        # Kontrola program tagu xml
        if root.tag != "program":
            exit(32)

        # Kontrola jazyka v xml tagu
        if root.items()[0][1] != "IPPcode22":
            exit(32)

        for inst in root:
            if inst.tag != "instruction":
                exit(32)
            for arg in inst:
                if arg.tag[:3].lower() != "arg":
                    exit(32)

        # Vytazeni instrukci do listu
        instructions = root.findall("instruction")

        # List ktery bude slouzit pro ulozeni instrukci a nasledne serazeni
        classList = []

        # Beh pro ulozeni do seznamu
        x = 0
        while x < len(instructions):
            instObj = Instruction(instructions[x])
            classList.append(instObj)
            x += 1

        # Serazeni seznamu
        classList.sort(key=lambda x: x.order)

        self.checkDuplicitOrder(classList)

        x = 0

        # Naplneni stacku labels
        for inst in classList:
            if inst.opcode.lower() == "label":
                # zalozeni objektu label
                labelObj = Label(x, inst.args[0][1])
                for obj in labels:
                    # Label jiz existuje
                    if obj.name == inst.args[0][1]:
                        exit(52)
                labels.append(labelObj)
                # print(labels[0].position)
            x += 1

        # Provadeni instrukci
        while Interpreter.count < len(classList):
            classList[Interpreter.count].execute()
            Interpreter.count += 1

        # for inst in classList:
        #     inst.execute()


    def checkDuplicitOrder(self, list):
        i = 0
        j = 0
        while i < len(list):
            if not (i == len(list)-1):
                if list[i].order == list[i+1].order:
                    exit(32)
            i += 1


class Instruction:
    def __init__(self, instruction):
        self.checkInstruction(instruction)

        self.order = int(instruction.items()[0][1])
        self.opcode = instruction.items()[1][1]
        self.args = []

        self.getArguments(instruction)

    # Do instruction objektu nacpe jeho argumenty do args
    def getArguments(self, instruction):
        for child in instruction:
            tmp = []
            tmp.append(child.items()[0][1])
            tmp.append(child.text)
            self.args.append(tmp)

    # vraci pouze int hodnotu
    def getIntValueFromArg(self, arg):
        # pokud je arg promenna, podle jmena si vytahne atom z listu
        if arg[0].lower() == "var":
            atom = programFrame.getAtomByName(arg[1])
            if atom.dataType.lower() != "int":
                exit(53)
            else:
                return atom.value
        # pokud je konstanta pouze vraci hodnotu
        elif arg[0].lower() == "int":
            return int(arg[1])
        else:
            exit(53)

    # Bere arg a naplnuje list myArr hodnotou a datovym typem
    def getValueTypeFromArg(self, arg, myArr):
        if arg[0].lower() == "var":
            atom = programFrame.getAtomByName(arg[1])
            myArr.append(atom.value)
            myArr.append(atom.dataType)
        elif arg[0].lower() == "int" or arg[0].lower() == "nil" or arg[0].lower() == "bool" \
                or arg[0].lower() == "string":
            myArr.append(arg[1])
            myArr.append(arg[0])
        else:
            exit(53)

    # vraci 1 pokud se typy rovnaji
    def isSameType(self, arg1, arg2):
        if arg1.lower() == arg2.lower():
            return 1
        else:
            return 0

    def isNil(self, arg):
        if arg.lower() == "nil":
            return 1

    def makeInt(self, arg1, arg2):
        arg1 = int(arg1)
        arg2 = int(arg2)

    # Porovnava dva <symb> podle argumentu operator rozhoduje jak bude hodnoty porovnavat
    def compareArgs(self, operator, arg1, arg2):
        #print(operator, arg1, arg2)
        firstArr = []
        secondArr = []
        self.getValueTypeFromArg(arg1, firstArr)
        self.getValueTypeFromArg(arg2, secondArr)

        # --LESSER THAN--
        if operator.lower() == "lt":
            # nesmi byt nil
            if self.isNil(firstArr[1]):
                exit(53)
            if self.isNil(secondArr[1]):
                exit(53)
            # musi byt stejny typ
            if not(self.isSameType(firstArr[1], secondArr[1])):
                exit(53)
            # podle typu porovna hodnoty
            if firstArr[1].lower() == "int":
                if int(firstArr[0]) < int(secondArr[0]):
                    return 1
            if firstArr[1].lower() == "string":
                if firstArr[0] < secondArr[0]:
                    return 1
            if firstArr[1].lower() == "bool":
                if firstArr[0].lower() < secondArr[0].lower():
                    return 1
        # --GREATER THAN--
        elif operator.lower() == "gt":
            # nesmi byt nil
            if self.isNil(firstArr[1]):
                exit(53)
            if self.isNil(secondArr[1]):
                exit(53)
            # musi byt stejny typ
            if not(self.isSameType(firstArr[1], secondArr[1])):
                exit(53)
            # podle typu porovna hodnoty
            if firstArr[1].lower() == "int":
                if int(firstArr[0]) > int(secondArr[0]):
                    return 1
            if firstArr[1].lower() == "string":
                if firstArr[0] > secondArr[0]:
                    return 1
            if firstArr[1].lower() == "bool":
                if firstArr[0].lower() > secondArr[0].lower():
                    return 1
        # --EQUAL--
        elif operator.lower() == "eq":
            # musi byt stejny typ
            if not(self.isSameType(firstArr[1], secondArr[1])):
                exit(53)
            # podle typu porovna hodnoty
            if firstArr[1].lower() == "int":
                if int(firstArr[0]) == int(secondArr[0]):
                    return 1
            if firstArr[1].lower() == "string":
                if firstArr[0] == secondArr[0]:
                    return 1
            if firstArr[1].lower() == "bool":
                if firstArr[0].lower() == secondArr[0].lower():
                    return 1
        # --JUMP IF EQUAL--
        elif operator.lower() == "jumpeq":
            # Porovnani dvou hodnot typu nil
            if self.isNil(firstArr[1]) and self.isNil(secondArr[1]):
                return 1
            # Porovnavani typu(bool,str,int) s nil
            elif self.isNil(firstArr[1]) or self.isNil(secondArr[1]):
                if firstArr[0].lower() == secondArr[0].lower():
                    return 1
            # porovnavani bez nil
            else:
                # musi byt stejny typ
                if self.isSameType(firstArr[1], secondArr[1]):
                    # podle typu porovna hodnoty
                    if firstArr[1].lower() == "int":
                        if int(firstArr[0]) == int(secondArr[0]):
                            return 1
                    elif firstArr[1].lower() == "string":
                        if firstArr[0] == secondArr[0]:
                            return 1
                    elif firstArr[1].lower() == "bool":
                        if firstArr[0].lower() == secondArr[0].lower():
                            return 1
        return 0

    # provadi porovnani podle logickych operatoru
    def logicCompareArgs(self, operator, arg1, arg2):
        firstArr = []
        secondArr = []
        operand1 = False
        operand2 = False

        # ulozi do firstArr hodnotu a datovyTyp
        self.getValueTypeFromArg(arg1, firstArr)

        # --NOT-- (pracuje s jednim argumentem)
        if operator.lower() == "not":
            if firstArr[1].lower() != "bool":
                exit(53)
            if firstArr[0].lower() == "false":
                return 1
            else:
                return 0

        # nacteni druheho argumentu pro ostatni logicke operatory
        self.getValueTypeFromArg(arg2, secondArr)

        # musi byt stejny typ
        if not(self.isSameType(firstArr[1], secondArr[1])):
            exit(53)
        if firstArr[1].lower() != "bool":
            exit(53)
        if operator.lower() == "and":
            if firstArr[0].lower() == "true" and secondArr[0].lower() == "true":
                return 1
        elif operator.lower() == "or":
            if firstArr[0].lower() == "true" or secondArr[0].lower() == "true":
                return 1
        return 0

    def printCorrect(self, arr):
        # Dle mailu nema pridavat odradkovani, ale prazny retecez s print("") text odradkuje, tak jsem usoudil ze podle
        # mailu ma byt tvar takto:
        if arr[1].lower() == "bool":
            print(arr[0].lower())
        elif arr[1].lower() == "nil":
            print("", end='')
        else:
            print(arr[0], end='')
            tmp = []

    def getLabelPos(self, arg):
        for label in labels:
            if arg == label.name:
                return label.position

    @staticmethod
    def checkInstruction(instruction):
        if not("order" in instruction.attrib) or not("opcode" in instruction.attrib):
            exit(32)
        if int(instruction.attrib["order"]) <= 0:
            exit(32)

    def execute(self):
        if self.opcode.lower() == "move":
            programFrame.setValue(self.args[0][1], self.args[1][0], self.args[1][1])
        elif self.opcode.lower() == "defvar":
            programFrame.addToFrame(self.args[0][1])
        elif self.opcode.lower() == "createframe":
            # zahodi docasny ramec
            if programFrame.TF != None:
                programFrame.TF.clear()
            programFrame.TF = []
        elif self.opcode.lower() == "pushframe":
            if programFrame.TF is None:
                exit(55)
            else:
                programFrame.LF.append(programFrame.TF)
                programFrame.TF = None
        elif self.opcode.lower() == "popframe":
            if not programFrame.LF:
                exit(55)
            else:
                programFrame.TF = programFrame.LF.pop()
        elif self.opcode.lower() == "call":
            programFrame.CallStack.append(Interpreter.count)
            jumpLabel = Label(None, self.args[0][1])
            if not(jumpLabel.inLabel()):
                exit(52)
            Interpreter.count = jumpLabel.position
        elif self.opcode.lower() == "return":
            if programFrame.CallStack:
                jumpBack = programFrame.CallStack.pop()
                Interpreter.count = jumpBack
            else:
                return
        elif self.opcode.lower() == "pushs":
            if self.args[0][0].lower() == "var":
                tmp = []
                atom = FrameAtom(None, None, None)
                atom = programFrame.getAtomByName(self.args[0][1])
                tmp.append(atom.value)
                tmp.append(atom.dataType)
                programFrame.DataStack.append(tmp)
            else:
                tmp = []
                tmp.append(self.args[0][0].lower())
                tmp.append(self.args[0][1])
                programFrame.DataStack.append(tmp)
        elif self.opcode.lower() == "pops":
            if not(programFrame.DataStack):
                exit(56)
            else:
                datatype, value = programFrame.DataStack.pop()
                #print(self.args[0][1])
                programFrame.setValue(self.args[0][1], datatype, value)
        elif self.opcode.lower() == "add":
            firstValue = self.getIntValueFromArg(self.args[1])
            secondValue = self.getIntValueFromArg(self.args[2])
            programFrame.setValue(self.args[0][1], "int", firstValue+secondValue)
        elif self.opcode.lower() == "sub":
            firstValue = self.getIntValueFromArg(self.args[1])
            secondValue = self.getIntValueFromArg(self.args[2])
            programFrame.setValue(self.args[0][1], "int", firstValue-secondValue)
        elif self.opcode.lower() == "mul":
            firstValue = self.getIntValueFromArg(self.args[1])
            secondValue = self.getIntValueFromArg(self.args[2])
            programFrame.setValue(self.args[0][1], "int", firstValue*secondValue)
        elif self.opcode.lower() == "idiv":
            firstValue = self.getIntValueFromArg(self.args[1])
            secondValue = self.getIntValueFromArg(self.args[2])
            if secondValue == 0:
                exit(57)
            programFrame.setValue(self.args[0][1], "int", int(firstValue/secondValue))
        elif self.opcode.lower() == "lt" or self.opcode.lower() == "gt" or self.opcode.lower() == "eq":
            if self.compareArgs(self.opcode, self.args[1], self.args[2]):
                programFrame.setValue(self.args[0][1], "bool", "true")
            else:
                programFrame.setValue(self.args[0][1], "bool", "false")
        elif self.opcode.lower() == "and" or self.opcode.lower() == "or":
            if self.logicCompareArgs(self.opcode, self.args[1], self.args[2]):
                programFrame.setValue(self.args[0][1], "bool", "true")
            else:
                programFrame.setValue(self.args[0][1], "bool", "false")
        elif self.opcode.lower() == "not":
            if self.logicCompareArgs(self.opcode, self.args[1], None):
                programFrame.setValue(self.args[0][1], "bool", "true")
            else:
                programFrame.setValue(self.args[0][1], "bool", "false")
        elif self.opcode.lower() == "int2char":
            value = self.getIntValueFromArg(self.args[1])
            programFrame.setValue(self.args[0][1], "string", chr(value))
        elif self.opcode.lower() == "stri2int":
            firstArr = []
            secondArr = []
            self.getValueTypeFromArg(self.args[1], firstArr)
            self.getValueTypeFromArg(self.args[2], secondArr)
            if secondArr[1].lower() != "int":
                exit(53)
            if int(secondArr[0]) > len(firstArr[0])-1:
                exit(58)
            char = firstArr[0][int(secondArr[0])]
            programFrame.setValue(self.args[0][1], "int", ord(char))
        elif self.opcode.lower() == "read":
            readValue = Interpreter.ReadFrom.readline().strip()
            if self.args[1][1].lower() == "int":
                if re.match("^[+|-]?[0-9]+$", readValue):
                    programFrame.setValue(self.args[0][1], "int", readValue)
                else:
                    programFrame.setValue(self.args[0][1], "nil", "nil")
            elif self.args[1][1].lower() == "bool":
                if readValue.lower() == "true":
                    programFrame.setValue(self.args[0][1], "bool", "true")
                else:
                    programFrame.setValue(self.args[0][1], "bool", "false")
            else:
                programFrame.setValue(self.args[0][1], "string", readValue)
        elif self.opcode.lower() == "write":
            myArr = []
            self.getValueTypeFromArg(self.args[0], myArr)
            self.printCorrect(myArr)
        elif self.opcode.lower() == "concat":
            firstArr = []
            secondArr = []
            self.getValueTypeFromArg(self.args[1], firstArr)
            self.getValueTypeFromArg(self.args[2], secondArr)
            if firstArr[1].lower() != "string" or secondArr[1].lower() != "string":
                exit(58)
            if not(self.isSameType(firstArr[1], secondArr[1])):
                exit(53)
            programFrame.setValue(self.args[0][1], "string", firstArr[0]+secondArr[0])
        elif self.opcode.lower() == "strlen":
            myArr = []
            self.getValueTypeFromArg(self.args[1], myArr)
            programFrame.setValue(self.args[0][1], "int", len(myArr[0]))
        elif self.opcode.lower() == "getchar":
            firstArr = []
            secondArr = []
            self.getValueTypeFromArg(self.args[1], firstArr)
            self.getValueTypeFromArg(self.args[2], secondArr)
            if secondArr[1].lower() != "int":
                exit(53)
            if int(secondArr[0]) > len(firstArr[0]) - 1:
                exit(58)
            char = firstArr[0][int(secondArr[0])]
            programFrame.setValue(self.args[0][1], "string", char)
            #
        elif self.opcode.lower() == "setchar":
            # ziskani atomu z ramce
            atom = programFrame.getAtomByName(self.args[0][1])
            if atom.dataType.lower() != "string":
                exit(53)
            # jmeno co budu modifikovat
            value = atom.value
            # vytazeni pozice
            position = self.getIntValueFromArg(self.args[1])
            # kontrola zda nejsem mimo index
            if position > len(value)-1:
                exit(58)
            # vytazeni retezce ktery obsahuje char kterym budeme nahrazovat
            # pomocny array pro uchovani hodnot
            myArr = []
            self.getValueTypeFromArg(self.args[2], myArr)
            char = myArr[0][0]
            value = value[:position] + char + value[position+1:]
            programFrame.setValue(self.args[0][1], "string", value)
        elif self.opcode.lower() == "type":
            if self.args[1][0] == "var":
                frame = self.args[1][1][:2]
                name = self.args[1][1][3:]
                if programFrame.inFrame(frame, name):
                    getatomtype = programFrame.getAtomByName(self.args[1][1])
                    programFrame.setValue(self.args[0][1], "string", getatomtype.dataType)
                    atom = programFrame.getAtomByName(self.args[0][1])
                else:
                    programFrame.setValue(self.args[0][1], "string", "")
            else:
                myArr = []
                self.getValueTypeFromArg(self.args[1], myArr)
                programFrame.setValue(self.args[0][1], "string", myArr[1])
        elif self.opcode.lower() == "jump":
            Interpreter.count = self.getLabelPos(self.args[0][1])
        elif self.opcode.lower() == "jumpifeq":
            if self.compareArgs("jumpeq", self.args[1], self.args[2]):
                label = Label(None, self.args[0][1])
                if label.inLabel():
                    Interpreter.count = self.getLabelPos(self.args[0][1])

        elif self.opcode.lower() == "jumpifneq":
            if not(self.compareArgs("jumpeq", self.args[1], self.args[2])):
                label = Label(None, self.args[0][1])
                if label.inLabel():
                    Interpreter.count = self.getLabelPos(self.args[0][1])

        elif self.opcode.lower() == "exit":
            value = self.getIntValueFromArg(self.args[0])
            if 49 >= value >= 0:
                exit(value)
            else:
                exit(57)
        elif self.opcode.lower() == "dprint":
            myArr = []
            self.getValueTypeFromArg(self.args[0], myArr)
            print(myArr[0], file=sys.stderr)
        elif self.opcode.lower() == "break":
            print("Poradi aktualni instrukce: ",Interpreter.count, file=sys.stderr)
            print("Obsah globalniho ramce:", programFrame.GF, file=sys.stderr)
            print("Obsah lokalniho ramce: ", programFrame.LF, file=sys.stderr)
            print("Obsah docasneho ramce: ", programFrame.TF, file=sys.stderr)
        else:
            exit(32)


class Frame:
    GF = []
    TF = None
    LF = []
    Stack = []
    CallStack = []
    DataStack = []

    def inFrame(self, frame, name):
        if frame == "GF":
            name = self.getName(name)
            for atom in self.GF:
                if atom.name == name:
                    return 1
        elif frame == "TF":
            if self.TF is not None:
                for atom in self.GF:
                    if atom.name == name:
                        return 1
        elif frame == "TF":
            if self.LF:
                for atom in self.LF[-1]:
                    if atom.name == name:
                        return 1
        else:
            return 0

    # #TODO DODELAT -> TODO DODELAT
    def getAtomByName(self, arg):
        name = self.getName(arg)
        frame = self.getFrame(arg)
        if not(self.inFrame(frame, name)):
            exit(54)
        atom = self.getAtomByFrameName(frame, name)
        return atom

    def getAtomByFrameName(self, frame, name):
        if frame == "GF":
            for atom in programFrame.GF:
                if atom.name == name:
                    return atom

    # Ziskam si do ktereho frame chci addovat -> Podivam se jestli v danem ramci "atom" neexistuje
    def addToFrame(self, arg):
        myFrame = self.getFrame(arg)
        atomName = self.getName(arg)
        atom = FrameAtom(None, atomName, None)
        if self.inFrame(myFrame, arg):
            exit(52)
            # TODO ODDELAT LOWER A PRIDAT CHYBU
        if myFrame.lower() == "gf":
            self.GF.append(atom)
        elif myFrame.lower() == "lf":
            self.LF.append(atom)
        elif myFrame.lower() == "tf":
            self.TF.append(atom)

    def setValue(self, replaceMe, dataType, value):
        frame = self.getFrame(replaceMe)

        replaceMe = self.getName(replaceMe)
        #
        if frame.lower() == "gf":
            if not (self.inFrame(self.getFrame(replaceMe), replaceMe)):
                exit(54)
            for atom in self.GF:
                if atom.name == replaceMe:
                    atom.updateAtomValue(value, dataType)
        elif frame.lower() == "lf":
            if not (self.inFrame(self.getFrame(replaceMe), replaceMe)):
                exit(54)
            if not(self.LF):
                exit(55)
            for atom in self.LF:
                if atom.name == replaceMe:
                    atom.updateAtomValue(value, dataType)
        elif frame.lower() == "tf":
            if not (self.inFrame(self.getFrame(replaceMe), replaceMe)):
                exit(54)
            if self.TF is None:
                exit(55)
            for atom in self.TF:
                if atom.name == replaceMe:
                    atom.updateAtomValue(value, dataType)

    def getFrame(self, value):
        frame = value[:2]
        return frame

    @staticmethod
    def getName(arg):
        # TODO dodelat pro vsechny ramce
        if arg[:3] != "GF@" or arg[:3] != "TF@" or arg[:3] != "LF@":
            return arg
        name = arg[3:]
        return name


# Type, value, name,
class FrameAtom:
    def __init__(self, datatype, name, value):
        self.dataType = datatype
        self.name = name
        self.value = value

    def updateAtomValue(self, value, datatype):
        tmp = []
        self.dataType = datatype
        if self.dataType.lower() == "int":
            self.value = int(value)
        else:
            self.value = value


class Label:
    def __init__(self, position, name):
        self.position = position
        self.name = name

    def inLabel(self):
        for label in labels:
            if self.name == label.name:
                self.position = label.position
                return 1
        return 0

def main():
    # Nastaveni argumentu pro argparse
    argumentParsed = argparse.ArgumentParser()
    argumentParsed.add_argument("--source", default=stdin, dest="source", nargs=1, help="INSERT HELP")
    argumentParsed.add_argument("--input", default=stdin, dest="input", nargs=1, help="INSERT HELP")

    # Vytazeni argumentu do promenne
    args = argumentParsed.parse_args()

    # Ulozeni jednotlivych souboru
    sourceFile = args.source
    inputFile = args.input

    # Kontrola vstupu
    checkFiles(sourceFile, inputFile)

    interpreter = Interpreter(sourceFile, inputFile)
    interpreter.parseTree()
    #print(programFrame.GF[1].name, programFrame.GF[1].dataType, programFrame.GF[1].value)

def checkFiles(source, input):
    # Pokud jsou oba soubory nacitany ze stdin = chyba
    # Pokud je source a input file stejny = chyba
    if source == input:
        exit(10)

    # Zkouska existence source souboru
    if source != stdin:
        if not (os.path.isfile(source[0])):
            exit(11)

    # Zkouska existence input souboru
    if input != stdin:
        if not (os.path.isfile(input[0])):
            exit(11)

labels = []
programFrame = Frame()
main()
