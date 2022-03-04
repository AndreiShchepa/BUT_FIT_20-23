import xml.etree.ElementTree as ET
import sys

class Program:
    def __init__(self):
        self._GF = dict()
        self._LF = None
        self._TF = None
        self._frames = []


class Instruction:
    instruction_list = []

    def __init__(self, opcode: str, num: int, dict_args):
        self._opcode: str = opcode
        self._num_of_arg: int = num
        self._arguments_dict: dict() = dict_args
        self.instruction_list.append(self)

    def get_opcode(self):
        return self._opcode

    def get_num_of_arg(self):
        return self._num_of_arg

    def get_args_dict(self):
        return self._arguments_dict

class Defvar(Instruction):
    def __init__(self, dict_args):
        super().__init__("DEFVAR", 1, dict_args)

    def check_args(self):
        num = super().get_num_of_arg()

    def execute(self):
        print("Defvar is here!\n")

class Move(Instruction):
    def __init__(self, dict_args):
        super().__init__("MOVE", 2, dict_args)

    def check_args(self):
        num = super().get_num_of_arg()

    def execute(self):
        print("Move is here!\n")

class Createframe(Instruction, Program):
    def __init__(self, dict_args):
        super().__init__("CREATEFRAME", 0, dict_args)

    def check_args(self):
        num = super().get_num_of_arg()

    def execute(self):
        self._TF = dict()
        print("Createframe is here!\n")

class Popframe(Instruction):
    def __init__(self, dict_args):
        super().__init__("POPFRAME", 0, dict_args)

    def check_args(self):
        num = super().get_num_of_arg()

    def execute(self):
        print("Popframe is here!\n")

class Break(Instruction):
    def __init__(self, dict_args):
        super().__init__("BREAK", 0, dict_args)

    def check_args(self):
        num = super().get_num_of_arg()

    def execute(self):
        print("Break is here!\n")

class Call(Instruction):
    def __init__(self, dict_args):
        super().__init__("CALL", 1, dict_args)

    def check_args(self):
        num = super().get_num_of_arg()

    def execute(self):
        print("Move is here!\n")

class Jump(Instruction):
    def __init__(self, dict_args):
        super().__init__("JUMP", 1, dict_args)

    def check_args(self):
        num = super().get_num_of_arg()

    def execute(self):
        print("Jump is here!\n")

class Pushs(Instruction):
    def __init__(self, dict_args):
        super().__init__("PUSHS", 1, dict_args)

    def check_args(self):
        num = super().get_num_of_arg()

    def execute(self):
        print("Pushs is here!\n")

class Exit(Instruction):
    def __init__(self, dict_args):
        super().__init__("EXIT", 1, dict_args)

    def check_args(self):
        num = super().get_num_of_arg()

    def execute(self):
        print("Exit is here!\n")

class Jumpifeq(Instruction):
    def __init__(self, dict_args):
        super().__init__("JUMPIFEQ", 3, dict_args)

    def check_args(self):
        num = super().get_num_of_arg()

    def execute(self):
        print("Jumpifeq is here!\n")

class Add(Instruction):
    def __init__(self, dict_args):
        super().__init__("ADD", 3, dict_args)

    def check_args(self):
        num = super().get_num_of_arg()

    def execute(self):
        print("Add is here!\n")

class Mul(Instruction):
    def __init__(self, dict_args):
        super().__init__("MUL", 3, dict_args)

    def check_args(self):
        num = super().get_num_of_arg()

    def execute(self):
        print("Mul is here!\n")

class Lt(Instruction):
    def __init__(self, dict_args):
        super().__init__("LT", 3, dict_args)

    def check_args(self):
        num = super().get_num_of_arg()

    def execute(self):
        print("Lt is here!\n")

class Eq(Instruction):
    def __init__(self, dict_args):
        super().__init__("EQ", 3, dict_args)

    def check_args(self):
        num = super().get_num_of_arg()

    def execute(self):
        print("Eq is here!\n")

class Or(Instruction):
    def __init__(self, dict_args):
        super().__init__("Or", 3, dict_args)

    def check_args(self):
        num = super().get_num_of_arg()

    def execute(self):
        print("Or is here!\n")

class Concat(Instruction):
    def __init__(self, dict_args):
        super().__init__("CONCAT", 3, dict_args)

    def check_args(self):
        num = super().get_num_of_arg()

    def execute(self):
        print("Concat is here!\n")

class Setchar(Instruction):
    def __init__(self, dict_args):
        super().__init__("SETCHAR", 3, dict_args)

    def check_args(self):
        num = super().get_num_of_arg()

    def execute(self):
        print("Setchar is here!\n")

class Stri2int(Instruction):
    def __init__(self, dict_args):
        super().__init__("STRI2INT", 3, dict_args)

    def check_args(self):
        num = super().get_num_of_arg()

    def execute(self):
        print("Stri2int is here!\n")

class Pops(Instruction):
    def __init__(self, dict_args):
        super().__init__("POPS", 1, dict_args)

    def check_args(self):
        num = super().get_num_of_arg()

    def execute(self):
        print("Pops is here!\n")

class Pushframe(Instruction):
    def __init__(self, dict_args):
        super().__init__("PUSHFRAME", 0, dict_args)

    def check_args(self):
        num = super().get_num_of_arg()

    def execute(self):
        print("Pushframe is here!\n")

class Return(Instruction):
    def __init__(self, dict_args):
        super().__init__("Return", 0, dict_args)

    def check_args(self):
        num = super().get_num_of_arg()

    def execute(self):
        print("Return is here!\n")

class Strlen(Instruction):
    def __init__(self, dict_args):
        super().__init__("STRLEN", 2, dict_args)

    def check_args(self):
        num = super().get_num_of_arg()

    def execute(self):
        print("Strlen is here!\n")

class Label(Instruction):
    def __init__(self, dict_args):
        super().__init__("LABEL", 1, dict_args)

    def check_args(self):
        num = super().get_num_of_arg()

    def execute(self):
        print("Label is here!\n")

class Type(Instruction):
    def __init__(self, dict_args):
        super().__init__("TYPE", 2, dict_args)

    def check_args(self):
        num = super().get_num_of_arg()

    def execute(self):
        print("Type is here!\n")

class Write(Instruction):
    def __init__(self, dict_args):
        super().__init__("WRITE", 1, dict_args)

    def check_args(self):
        num = super().get_num_of_arg()

    def execute(self):
        (key, value), = self.get_args_dict().items()
        print("" if key == "nil" else value, end='', file=sys.stdout)

class Dprint(Instruction):
    def __init__(self, dict_args):
        super().__init__("DPRINT", 1, dict_args)

    def check_args(self):
        num = super().get_num_of_arg()

    def execute(self):
        print("Dprint is here!\n")

class Int2char(Instruction):
    def __init__(self, dict_args):
        super().__init__("INT2CHAR", 2, dict_args)

    def check_args(self):
        num = super().get_num_of_arg()

    def execute(self):
        print("Int2char is here!\n")

class Jumpifneq(Instruction):
    def __init__(self, dict_args):
        super().__init__("JUMPIFNEQ", 3, dict_args)

    def check_args(self):
        num = super().get_num_of_arg()

    def execute(self):
        print("Jumpifneq is here!\n")

class Sub(Instruction):
    def __init__(self, dict_args):
        super().__init__("SUB", 3, dict_args)

    def check_args(self):
        num = super().get_num_of_arg()

    def execute(self):
        print("Sub is here!\n")

class Idiv(Instruction):
    def __init__(self, dict_args):
        super().__init__("IDIV", 3, dict_args)

    def check_args(self):
        num = super().get_num_of_arg()

    def execute(self):
        print("Idiv is here!\n")

class Gt(Instruction):
    def __init__(self, dict_args):
        super().__init__("GT", 3, dict_args)

    def check_args(self):
        num = super().get_num_of_arg()

    def execute(self):
        print("Gt is here!\n")

class And(Instruction):
    def __init__(self, dict_args):
        super().__init__("AND", 3, dict_args)

    def check_args(self):
        num = super().get_num_of_arg()

    def execute(self):
        print("And is here!\n")

class Not(Instruction):
    def __init__(self, dict_args):
        super().__init__("NOT", 2, dict_args)

    def check_args(self):
        num = super().get_num_of_arg()

    def execute(self):
        print("Not is here!\n")

class Getchar(Instruction):
    def __init__(self, dict_args):
        super().__init__("GETCHAR", 3, dict_args)

    def check_args(self):
        num = super().get_num_of_arg()

    def execute(self):
        print("Getchar is here!\n")

class Read(Instruction):
    def __init__(self, dict_args):
        super().__init__("READ", 2, dict_args)

    def check_args(self):
        num = super().get_num_of_arg()

    def execute(self):
        print("Read is here!\n")

class Argument:
    def __init__(self, type_arg: str, str_arg):
        self._type = type_arg
        self._arg = str_arg

class Factory:
    _dict_func = {"DEFVAR"  : Defvar , "CREATEFRAME" : Createframe, "POPFRAME" : Popframe,
                  "BREAK"   : Break  , "CALL"        : Call       , "JUMP"     : Jump    ,
                  "EXIT"    : Exit   , "MOVE"        : Move       , "ADD"      : Add     ,
                  "CONCAT"  : Concat , "STRI2INT"    : Stri2int   , "LT"       : Lt      ,
                  "RETURN"  : Return , "INT2CHAR"    : Int2char   , "EQ"       : Eq      ,
                  "STRLEN"  : Strlen , "PUSHFRAME"   : Pushframe  , "OR"       : Or      ,
                  "DPRINT"  : Dprint , "JUMPIFNEQ"   : Jumpifneq  , "POPS"     : Pops    ,
                  "GETCHAR" : Getchar, "JUMPIFEQ"    : Jumpifeq   , "LABEL"    : Label   ,
                  "WRITE"   : Write  , "TYPE"        : Type       , "IDIV"     : Idiv    ,
                  "READ"    : Read   , "AND"         : And        , "GT"       : Gt      ,
                  "PUSHS"   : Pushs  , "MUL"         : Mul        , "SUB"      : Sub     ,
                  "SETCHAR" : Setchar, "NOT"         : Not
                  }

    @classmethod
    def resolve(cls, string: str, dict_args: dict()):
        try:
            return cls._dict_func[string](dict_args)
        except:
            print("ERROR with name of instruction, add exit error")


# MAIN
tree = ET.parse('out.xml')
root = tree.getroot()

for child in root:
    dict_args = dict()
    for ch in child:
        dict_args[ch.attrib["type"]] = ch.text

    instr = Factory.resolve(child.attrib["opcode"], dict_args)

for val in Instruction.instruction_list:
    #print(val.get_opcode(), "  ", val.get_args_dict())
    val.execute()
