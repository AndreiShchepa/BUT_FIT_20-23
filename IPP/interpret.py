import xml.etree.ElementTree as ET
import sys
import re
import getopt

PARAM_ERR = 10
INPUT_ERR = 11
OUTPUT_ERR = 12
FORMAT_ERR = 31
STRUCT_ERR = 32
SEMANTIC_ERR = 52
TYPES_ERR = 53
UNDECLARE_ERR = 54
FRAME_ERR = 55
NOVALUE_ERR = 56
WRONG_VALUE_ERR = 57
STRING_ERR = 58

class Stats():
    def __init__(self):
        self._insts = 0
        self._hot = 0
        self._vars = 0
        self._insts_dict = dict()

class Program:
    def __init__(self):
        self._GF = dict()
        self._LF = None
        self._TF = None
        self._frames = []
        self._labels_dict = dict()
        self._return_lines = []
        self._line = 0
        self._num_of_instrs = 0
        self._data_stack = []
        self._stats = Stats()
        self._input_f = None
        self._order = [0] # change in case

    def get_line(self):
        return self._line

    def set_line(self, val):
        self._line = val

    def get_num_of_instrs(self):
        return self._num_of_instrs

    def set_num_of_instrs(self, val):
        self._num_of_instrs = val

    def open_file(self, file):
        self._input_f = open(file, 'r')

class Instruction():
    _instruction_list = []

    def __init__(self, opcode: str, num: int, dict_args):
        self._opcode: str = opcode
        self._num_of_arg: int = num
        self._arguments_dict: dict() = dict_args
        self._instruction_list.append(self)

    def get_opcode(self):
        return self._opcode

    def get_num_of_arg(self):
        return self._num_of_arg

    def get_arg(self, id):
        return self._arguments_dict[id]

    @classmethod
    def get_instr_list(cls):
        return cls._instruction_list

    def check_frame_none(self, frame):
        if frame == None:
            exit(FRAME_ERR)

    def _get_var(self, var, prg, key):
        if key != "var":
            return None, key, var

        #print(var)
        if var[0:2] == "LF":
            self.check_frame_none(prg._LF)
        elif var[0:2] == "TF":
            self.check_frame_none(prg._TF)
        try:
            if var[0:2] == "LF":
                type_value = prg._LF[var[3:]]
            elif var[0:2] == "GF":
                type_value = prg._GF[var[3:]]
            elif var[0:2] == "TF":
                type_value = prg._TF[var[3:]]
        except:
            return None, None, None

        typ, value = type_value[0], type_value[1]
        return var[3:], typ, value

    def _add_var(self, name, frame, typ, value, prg):
        if frame == "GF":
            prg._GF[name] = [typ, value]
        elif frame == "LF":
            self.check_frame_none(prg._LF)
            prg._LF[name] = [typ, value]
        elif frame == "TF":
            self.check_frame_none(prg._TF)
            prg._TF[name] = [typ, value]

class Defvar(Instruction):
    def __init__(self, dict_args):
        super().__init__("DEFVAR", 1, dict_args)

    def check_args(self):
        num = super().get_num_of_arg()

    def execute(self, prg):
        (key, value), = self.get_arg(0).items()
        frame, new_name = value[0:2], value[3:]
        name, typ, value = self._get_var(value, prg, key)

        if name != None or typ != None or value != None:
            #print("Redeclarartion errr")
            exit(SEMANTIC_ERR)

        self._add_var(new_name, frame, None, value, prg)

class Move(Instruction):
    def __init__(self, dict_args):
        super().__init__("MOVE", 2, dict_args)

    def check_args(self):
        num = super().get_num_of_arg()

    def execute(self, prg):
        frame1, name1, typ1, value1 = get_values(self, 0)
        where_is_name(name1)

        frame2, name2, typ2, value2 = get_values(self, 1)

        if typ1 == None or typ1 == typ2:
            self._add_var(name1, frame1, typ2, value2, prg)
        else:
            #print("SE")
            exit(TYPES_ERR)

class Createframe(Instruction):
    def __init__(self, dict_args):
        super().__init__("CREATEFRAME", 0, dict_args)

    def check_args(self):
        num = super().get_num_of_arg()

    def execute(self, prg):
        prg._TF = dict()

class Popframe(Instruction):
    def __init__(self, dict_args):
        super().__init__("POPFRAME", 0, dict_args)

    def check_args(self):
        num = super().get_num_of_arg()

    def execute(self, prg):
        if len(prg._frames) == 0:
            #print("ERRRRRRRRRRRRRR HOP")
            exit(FRAME_ERR)

        prg._TF = prg._frames.pop()
        prg._LF = prg._frames[-1]

class Break(Instruction):
    def __init__(self, dict_args):
        super().__init__("BREAK", 0, dict_args)

    def check_args(self):
        num = super().get_num_of_arg()

    def execute(self, prg):
        print("Current position: "    + prg._line    + "\n" + \
              "Global frame: "        + prg._GF      + "\n" + \
              "Local frame: "         + prg._LF      + "\n" + \
              "Temporary frame: "     + prg._TF      + "\n" + \
              "Instruction counter: " + prg._stats._insts, file=sys.stderr)

class Call(Instruction):
    def __init__(self, dict_args):
        super().__init__("CALL", 1, dict_args)

    def check_args(self):
        num = super().get_num_of_arg()

    def execute(self, prg):
        (key, value), = self.get_arg(0).items()

        try:
            prg._return_lines.append(prg.get_line())
            prg.set_line(prg._labels_dict[value] - 1)
        except:
            #print("Label doenst exist")
            exit(SEMANTIC_ERR)

class Jump(Instruction):
    def __init__(self, dict_args):
        super().__init__("JUMP", 1, dict_args)

    def check_args(self):
        num = super().get_num_of_arg()

    def execute(self, prg):
        (key, value), = self.get_arg(0).items()

        try:
            prg.set_line(prg._labels_dict[value] - 1)
        except:
            #print("Label doenst exist")
            exit(SEMANTIC_ERR)

class Pushs(Instruction):
    def __init__(self, dict_args):
        super().__init__("PUSHS", 1, dict_args)

    def check_args(self):
        num = super().get_num_of_arg()

    def execute(self, prg):
        (key, value), = self.get_arg(0).items()
        frame = value[0:2]
        name, typ, value = self._get_var(value, prg, key)

        if name == None and typ == None and value == None:
            #print("Not defind var")
            exit(UNDECLARE_ERR)

        prg._data_stack.append((typ, value))

class Exit(Instruction):
    def __init__(self, dict_args):
        super().__init__("EXIT", 1, dict_args)

    def check_args(self):
        num = super().get_num_of_arg()

    def execute(self, prg):
        (key, value), = self.get_arg(0).items()
        frame = value[0:2]
        name, typ, value = self._get_var(value, prg, key)

        # TODO
        if key == "var":
            print("smth")
        elif key != "int" or int(value) > 49 or int(value) < 0:
            #print("err with exit cmd")
            exit(WRONG_VALUE_ERR)

        print("write stats")
        exit(int(value))

class Jumpifeq(Instruction):
    def __init__(self, dict_args):
        super().__init__("JUMPIFEQ", 3, dict_args)

    def check_args(self):
        num = super().get_num_of_arg()

    def execute(self, prg):
        (key, value), = self.get_arg(0).items()
        frame2, name2, typ2, value2 = get_values(self, 1)
        frame3, name3, typ3, value3 = get_values(self, 2)

        if typ2 != typ3:
            #print("TYPES_ERR")
            exit(TYPES_ERR)

        try:
            if value2 == value3:
                prg.set_line(prg._labels_dict[value] - 1)
        except:
            #print("Label doenst exist")
            exit(SEMANTIC_ERR)

class Add(Instruction):
    def __init__(self, dict_args):
        super().__init__("ADD", 3, dict_args)

    def check_args(self):
        num = super().get_num_of_arg()

    def execute(self, prg):
        frame1, name1, typ1, value1 = get_values(self, 0)
        where_is_name(name1)

        frame2, name2, typ2, value2 = get_values(self, 1)
        frame3, name3, typ3, value3 = get_values(self, 2)

        check_type(typ1, typ2, typ3, "int")
        self._add_var(name1, frame1, "int", str(int(value2) + int(value3)), prg)

class Mul(Instruction):
    def __init__(self, dict_args):
        super().__init__("MUL", 3, dict_args)

    def check_args(self):
        num = super().get_num_of_arg()

    def execute(self, prg):
        frame1, name1, typ1, value1 = get_values(self, 0)
        where_is_name(name1)

        frame2, name2, typ2, value2 = get_values(self, 1)
        frame3, name3, typ3, value3 = get_values(self, 2)

        check_type(typ1, typ2, typ3, "int")
        self._add_var(name1, frame1, "int", str(int(value2) * int(value3)), prg)

class Lt(Instruction):
    def __init__(self, dict_args):
        super().__init__("LT", 3, dict_args)

    def check_args(self):
        num = super().get_num_of_arg()

    def execute(self, prg):
        frame1, name1, typ1, value1 = get_values(self, 0)
        where_is_name(name1)

        frame2, name2, typ2, value2 = get_values(self, 1)
        frame3, name3, typ3, value3 = get_values(self, 2)

        check_type(typ1, typ2, typ3, typ2)
        self._add_var(name1, frame1, "bool", str(value2 < value3).lower(), prg)

class Eq(Instruction):
    def __init__(self, dict_args):
        super().__init__("EQ", 3, dict_args)

    def check_args(self):
        num = super().get_num_of_arg()

    def execute(self, prg):
        frame1, name1, typ1, value1 = get_values(self, 0)
        where_is_name(name1)

        frame2, name2, typ2, value2 = get_values(self, 1)
        frame3, name3, typ3, value3 = get_values(self, 2)

        if typ2 != typ3 and typ2 != "nil" and typ3 != "nil":
            exit(TYPES_ERR)

        if typ1 == None or typ1 == "bool":
            self._add_var(name1, frame1, "bool", str(value2 == value3).lower(), prg)
        else:
            exit(TYPES_ERR)

class Or(Instruction):
    def __init__(self, dict_args):
        super().__init__("Or", 3, dict_args)

    def check_args(self):
        num = super().get_num_of_arg()

    def execute(self, prg):
        frame1, name1, typ1, value1 = get_values(self, 0)
        where_is_name(name1)

        frame2, name2, typ2, value2 = get_values(self, 1)
        frame3, name3, typ3, value3 = get_values(self, 2)

        check_type(typ1, typ2, typ3, "bool")
        self._add_var(name1, frame1, "bool", str(str2bool(value2) or str2bool(value3)).lower(), prg)

class Concat(Instruction):
    def __init__(self, dict_args):
        super().__init__("CONCAT", 3, dict_args)

    def check_args(self):
        num = super().get_num_of_arg()

    def execute(self, prg):
        frame1, name1, typ1, value1 = get_values(self, 0)
        where_is_name(name1)

        frame2, name2, typ2, value2 = get_values(self, 1)
        frame3, name3, typ3, value3 = get_values(self, 2)

        check_type(typ1, typ2, typ3, "string")
        self._add_var(name1, frame1, "string", value2 * value3, prg)

class Setchar(Instruction):
    def __init__(self, dict_args):
        super().__init__("SETCHAR", 3, dict_args)

    def check_args(self):
        num = super().get_num_of_arg()

    def execute(self, prg):
        frame1, name1, typ1, value1 = get_values(self, 0)
        where_is_name(name1)

        frame2, name2, typ2, value2 = get_values(self, 1)
        frame3, name3, typ3, value3 = get_values(self, 2)

        check_2_keys(typ1, "string", typ3, "string")
        if typ2 != "int" or int(value2) < 0:
            #print("Last errrrrrrr")
            exit(TYPES_ERR)

        try:
            value1[int(value2)] = value3[int(value2)]
            self._add_var(name1, frame1, "string", value1, prg)
        except:
            print("Mimo pole")
            exit(STRING_ERR)

        self._add_var(name1, frame1, "int", str(int(value2) + int(value3)), prg)

class Stri2int(Instruction):
    def __init__(self, dict_args):
        super().__init__("STRI2INT", 3, dict_args)

    def check_args(self):
        num = super().get_num_of_arg()

    def execute(self, prg):
        frame1, name1, typ1, value1 = get_values(self, 0)
        where_is_name(name1)

        frame2, name2, typ2, value2 = get_values(self, 1)
        frame3, name3, typ3, value3 = get_values(self, 2)

        check_2_keys(typ2, "string", typ3, "int")
        if typ1 != "string" and typ1 != None:
            #print("Sem ertggggg")
            exit(TYPES_ERR)

        try:
            self._add_var(name1, frame1, "string", str(ord(value2[value3])), prg)
        except:
            #print("Mimo pole")
            exit(STRING_ERR)

class Pops(Instruction):
    def __init__(self, dict_args):
        super().__init__("POPS", 1, dict_args)

    def check_args(self):
        num = super().get_num_of_arg()

    def execute(self, prg):
        if len(prg._data_stack) == 0:
            #print("ERORO POP")
            exit(NOVALUE_ERR)

        frame, name, typ, value = get_values(self, 0)
        type_stack, value_stack = prg._data_stack.pop()

        if typ != None and typ != type_stack:
            #print("SEM_ERR pop")
            exit(TYPES_ERR)

        self._add_var(name, frame, type_stack, value_stack, prg)

class Pushframe(Instruction):
    def __init__(self, dict_args):
        super().__init__("PUSHFRAME", 0, dict_args)

    def check_args(self):
        num = super().get_num_of_arg()

    def execute(self, prg):
        if prg._TF is None:
            #print("Not defined Temporary frame, exit with err")
            exit(FRAME_ERR)

        prg._frames.append(prg._TF)
        prg._LF = prg._TF
        prg._TF = None

class Return(Instruction):
    def __init__(self, dict_args):
        super().__init__("Return", 0, dict_args)

    def check_args(self):
        num = super().get_num_of_arg()

    def execute(self, prg):
        if len(prg._return_lines) == 0:
            #print("VUT FIT U ERR")
            exit(NOVALUE_ERR)

        prg.set_line(prg._return_lines.pop())

class Strlen(Instruction):
    def __init__(self, dict_args):
        super().__init__("STRLEN", 2, dict_args)

    def check_args(self):
        num = super().get_num_of_arg()

    def execute(self, prg):
        frame1, name1, typ1, value1 = get_values(self, 0)
        where_is_name(name1)

        frame2, name2, typ2, value2 = get_values(self, 1)

        check_2_keys(key1, "int", key2, "string")
        self._add_var(name1, frame, "int", strlen(value2), prg)

class Label(Instruction):
    def __init__(self, dict_args):
        super().__init__("LABEL", 1, dict_args)

    def check_args(self):
        num = super().get_num_of_arg()

    def execute(self, prg):
        pass

class Type(Instruction):
    def __init__(self, dict_args):
        super().__init__("TYPE", 2, dict_args)

    def check_args(self):
        num = super().get_num_of_arg()

    def execute(self, prg):
        frame1, name1, typ1, value1 = get_values(self, 0)
        where_is_name(name1)

        frame2, name2, typ2, value2 = get_values(self, 1)

        if typ1 != "string" and typ1 != None:
            #print("Semantic err")
            exit(TYPES_ERR)

        typ2 = "" if typ2 == None else typ2

        #if typ2

        self._add_var(name1, frame1, "string", typ2, prg)

class Write(Instruction):
    def __init__(self, dict_args):
        super().__init__("WRITE", 1, dict_args)

    def check_args(self):
        num = super().get_num_of_arg()

    def execute(self, prg):
        frame, name, typ, value = get_values(self, 0)

        #print(value, typ)

        value = value if "string" != typ else re.sub(r'\\([0-9]{3})', lambda x: chr(int(x[1])), value)
        print("" if typ == "nil" else value, end='', file=sys.stdout)

class Dprint(Instruction):
    def __init__(self, dict_args):
        super().__init__("DPRINT", 1, dict_args)

    def check_args(self):
        num = super().get_num_of_arg()

    def execute(self, prg):
        frame, name, typ, value = get_values(self, 0)

        value = value if "string" != typ else re.sub(r'\\([0-9]{3})', lambda x: chr(int(x[1])), value)
        print("" if typ == "nil" else value, end='', file=sys.stderr)

class Int2char(Instruction):
    def __init__(self, dict_args):
        super().__init__("INT2CHAR", 2, dict_args)

    def check_args(self):
        num = super().get_num_of_arg()

    def execute(self, prg):
        frame1, name1, typ1, value1 = get_values(self, 0)
        where_is_name(name1)

        frame2, name2, typ2, value2 = get_values(self, 1)

        check_2_keys(typ1, "string", typ2, "int")
        if 0 <= value2 <= 1114111:
            #print("Nejaka chybaaaa")
            exit(STRING_ERR)

        self._add_var(name1, frame1, "int", str(chr(value2)), prg)

class Jumpifneq(Instruction):
    def __init__(self, dict_args):
        super().__init__("JUMPIFNEQ", 3, dict_args)

    def check_args(self):
        num = super().get_num_of_arg()

    def execute(self, prg):
        (key, value), = self.get_arg(0).items()
        frame2, name2, typ2, value2 = get_values(self, 1)
        frame3, name3, typ3, value3 = get_values(self, 2)

        if typ2 != typ3:
            #print("SENMMNMMMM")
            exit(TYPES_ERR)

        try:
            if value2 != value3:
                prg.set_line(prg._labels_dict[value] - 1)
        except:
            #print("Label doenst exist")
            exit(SEMANTIC_ERR)

class Sub(Instruction):
    def __init__(self, dict_args):
        super().__init__("SUB", 3, dict_args)

    def check_args(self):
        num = super().get_num_of_arg()

    def execute(self, prg):
        frame1, name1, typ1, value1 = get_values(self, 0)
        where_is_name(name1)

        frame2, name2, typ2, value2 = get_values(self, 1)
        frame3, name3, typ3, value3 = get_values(self, 2)

        check_type(typ1, typ2, typ3, "int")
        self._add_var(name1, frame1, "int", str(int(value2) - int(value3)), prg)

class Idiv(Instruction):
    def __init__(self, dict_args):
        super().__init__("IDIV", 3, dict_args)

    def check_args(self):
        num = super().get_num_of_arg()

    def execute(self, prg):
        frame1, name1, typ1, value1 = get_values(self, 0)
        where_is_name(name1)

        frame2, name2, typ2, value2 = get_values(self, 1)
        frame3, name3, typ3, value3 = get_values(self, 2)

        check_type(typ1, typ2, typ3, "int")
        if int(value3) == 0:
            #print("Psel nahuy")
            exit(WRONG_VALUE_ERR)

        self._add_var(name1, frame1, "int", str(int(value2) // int(value3)), prg)

class Gt(Instruction):
    def __init__(self, dict_args):
        super().__init__("GT", 3, dict_args)

    def check_args(self):
        num = super().get_num_of_arg()

    def execute(self, prg):
        frame1, name1, typ1, value1 = get_values(self, 0)
        where_is_name(name1)

        frame2, name2, typ2, value2 = get_values(self, 1)
        frame3, name3, typ3, value3 = get_values(self, 2)

        check_type(typ1, typ2, typ3, typ2)
        self._add_var(name1, frame1, "bool", str(value2 > value3).lower(), prg)

class And(Instruction):
    def __init__(self, dict_args):
        super().__init__("AND", 3, dict_args)

    def check_args(self):
        num = super().get_num_of_arg()

    def execute(self, prg):
        frame1, name1, typ1, value1 = get_values(self, 0)
        where_is_name(name1)

        frame2, name2, typ2, value2 = get_values(self, 1)
        frame3, name3, typ3, value3 = get_values(self, 2)

        check_type(typ1, typ2, typ3, "bool")
        self._add_var(name1, frame1, "bool", str(str2bool(value2) and str2bool(value3)).lower(), prg)

class Not(Instruction):
    def __init__(self, dict_args):
        super().__init__("NOT", 2, dict_args)

    def check_args(self):
        num = super().get_num_of_arg()

    def execute(self, prg):
        frame1, name1, typ1, value1 = get_values(self, 0)
        where_is_name(name1)

        frame2, name2, typ2, value2 = get_values(self, 1)

        check_2_keys(typ1, "bool", typ2, "bool")
        self._add_var(name1, frame1, "bool", str(not str2bool(value2)).lower(), prg)

class Getchar(Instruction):
    def __init__(self, dict_args):
        super().__init__("GETCHAR", 3, dict_args)

    def check_args(self):
        num = super().get_num_of_arg()

    def execute(self, prg):
        frame1, name1, typ1, value1 = get_values(self, 0)
        where_is_name(name1)

        frame2, name2, typ2, value2 = get_values(self, 1)
        frame3, name3, typ3, value3 = get_values(self, 2)

        check_2_keys(typ2, "string", typ3, "int")
        if typ1 != "string" and typ1 != None:
            #print("Sem ertggggg")
            exit(TYPES_ERR)

        try:
            self._add_var(name1, frame1, "string", str(value2[value3]), prg)
        except:
            #print("Mimo pole")
            exit(STRING_ERR)

class Read(Instruction):
    def __init__(self, dict_args):
        super().__init__("READ", 2, dict_args)

    def check_args(self):
        num = super().get_num_of_arg()

    def execute(self, prg):
        line = input() if prg._input_f is None else prg._input_f.readline().rtrim()

        (key, value), = self.get_arg(1).items()
        frame1, name1, typ1, value1 = get_values(self, 0)
        where_is_name(name1)

        if value == 'int':
            self._add_var(name1, frame1, "int", str(int(line)), prg)
        elif value == 'string':
            self._add_var(name1, frame1, "string", str(line), prg)
        elif value == 'bool':
            new_v = "true" if line == "true" else "false"
            self._add_var(name1, frame1, "bool", new_v, prg)
        if value == '':
            self._add_var(name1, frame1, "nil", "nil", prg)

class AddS(Instruction):
    def __init__(self, dict_args):
        super().__init__("ADDS", 0, dict_args)

    def check_args(self):
        num = super().get_num_of_arg()

    def execute(self, prg):
        if len(prg._data_stack) < 2:
            #print("ERORO POP")
            exit(NOVALUE_ERR)

        type_stack1, value_stack1 = prg._data_stack.pop()
        type_stack2, value_stack2 = prg._data_stack.pop()

        if type_stack1 != type_stack2 or type_stack1 != "int":
            #print("SEM_ERR pop")
            exit(TYPES_ERR)

        prg._data_stack.append((type_stack1, int(value_stack1) + int(value_stack2)))

class SubS(Instruction):
    def __init__(self, dict_args):
        super().__init__("SUBS", 0, dict_args)

    def check_args(self):
        num = super().get_num_of_arg()

    def execute(self, prg):
        if len(prg._data_stack) < 2:
            #print("ERORO POP")
            exit(NOVALUE_ERR)

        type_stack1, value_stack1 = prg._data_stack.pop()
        type_stack2, value_stack2 = prg._data_stack.pop()

        if type_stack1 != type_stack2 or type_stack1 != "int":
            #print("SEM_ERR pop")
            exit(TYPES_ERR)

        prg._data_stack.append((type_stack1, int(value_stack1) - int(value_stack2)))

class MulS(Instruction):
    def __init__(self, dict_args):
        super().__init__("MULS", 0, dict_args)

    def check_args(self):
        num = super().get_num_of_arg()

    def execute(self, prg):
        if len(prg._data_stack) < 2:
            #print("ERORO POP")
            exit(NOVALUE_ERR)

        type_stack1, value_stack1 = prg._data_stack.pop()
        type_stack2, value_stack2 = prg._data_stack.pop()

        if type_stack1 != type_stack2 or type_stack1 != "int":
            #print("SEM_ERR pop")
            exit(TYPES_ERR)

        prg._data_stack.append((type_stack1, int(value_stack1) * int(value_stack2)))

class IdivS(Instruction):
    def __init__(self, dict_args):
        super().__init__("IDIVS", 0, dict_args)

    def check_args(self):
        num = super().get_num_of_arg()

    def execute(self, prg):
        if len(prg._data_stack) < 2:
            #print("ERORO POP")
            exit(NOVALUE_ERR)

        type_stack1, value_stack1 = prg._data_stack.pop()
        type_stack2, value_stack2 = prg._data_stack.pop()

        if type_stack1 != type_stack2 or type_stack1 != "int":
            #print("SEM_ERR pop")
            exit(TYPES_ERR)

        if int(value_stack2) == 0:
            #print("Psel nahuy")
            exit(WRONG_VALUE_ERR)

        prg._data_stack.append((type_stack1, int(value_stack1) // int(value_stack2)))

class ClearS(Instruction):
    def __init__(self, dict_args):
        super().__init__("CLEARS", 0, dict_args)

    def check_args(self):
        num = super().get_num_of_arg()

    def execute(self, prg):
        prg._data_stack = []

class JumpifeqS(Instruction):
    def __init__(self, dict_args):
        super().__init__("JUMPIFEQS", 1, dict_args)

    def check_args(self):
        num = super().get_num_of_arg()

    def execute(self, prg):
        (key, value), = self.get_arg(0).items()
        type_stack1, value_stack1 = prg._data_stack.pop()
        type_stack2, value_stack2 = prg._data_stack.pop()

        if type_stack1 != type_stack2:
            #print("TYPES_ERR")
            exit(TYPES_ERR)

        try:
            if value_stack1 == value_stack2:
                prg.set_line(prg._labels_dict[value] - 1)
        except:
            #print("Label doenst exist")
            exit(SEMANTIC_ERR)

class JumpifneqS(Instruction):
    def __init__(self, dict_args):
        super().__init__("JUMPIFNEQS", 1, dict_args)

    def check_args(self):
        num = super().get_num_of_arg()

    def execute(self, prg):
        (key, value), = self.get_arg(0).items()
        type_stack1, value_stack1 = prg._data_stack.pop()
        type_stack2, value_stack2 = prg._data_stack.pop()

        if type_stack1 != type_stack2:
            #print("TYPES_ERR")
            exit(TYPES_ERR)

        try:
            if value_stack1 != value_stack2:
                prg.set_line(prg._labels_dict[value] - 1)
        except:
            #print("Label doenst exist")
            exit(SEMANTIC_ERR)

class NotS(Instruction):
    def __init__(self, dict_args):
        super().__init__("NOTS", 0, dict_args)

    def check_args(self):
        num = super().get_num_of_arg()

    def execute(self, prg):
        if len(prg._data_stack) < 1:
            #print("ERORO POP")
            exit(NOVALUE_ERR)

        type_stack1, value_stack1 = prg._data_stack.pop()

        if type_stack1 != "bool":
            #print("SEM_ERR pop")
            exit(TYPES_ERR)

        prg._data_stack.append(("bool", str(not str2bool(type_stack1)).lower()))

class AndS(Instruction):
    def __init__(self, dict_args):
        super().__init__("ANDS", 0, dict_args)

    def check_args(self):
        num = super().get_num_of_arg()

    def execute(self, prg):
        if len(prg._data_stack) < 2:
            #print("ERORO POP")
            exit(NOVALUE_ERR)

        type_stack1, value_stack1 = prg._data_stack.pop()
        type_stack2, value_stack2 = prg._data_stack.pop()

        if type_stack1 != type_stack2 or type_stack1 != "bool":
            #print("SEM_ERR pop")
            exit(TYPES_ERR)

        prg._data_stack.append(("bool", str(str2bool(value_stack1) and str2bool(value_stack2)).lower()))

class OrS(Instruction):
    def __init__(self, dict_args):
        super().__init__("ORS", 0, dict_args)

    def check_args(self):
        num = super().get_num_of_arg()

    def execute(self, prg):
        if len(prg._data_stack) < 2:
            #print("ERORO POP")
            exit(NOVALUE_ERR)

        type_stack1, value_stack1 = prg._data_stack.pop()
        type_stack2, value_stack2 = prg._data_stack.pop()

        if type_stack1 != type_stack2 or type_stack1 != "bool":
            #print("SEM_ERR pop")
            exit(TYPES_ERR)

        prg._data_stack.append(("bool", str(str2bool(value_stack1) or str2bool(value_stack2)).lower()))

class EqS(Instruction):
    def __init__(self, dict_args):
        super().__init__("EQS", 0, dict_args)

    def check_args(self):
        num = super().get_num_of_arg()

    def execute(self, prg):
        if len(prg._data_stack) < 2:
            #print("ERORO POP")
            exit(NOVALUE_ERR)

        type_stack1, value_stack1 = prg._data_stack.pop()
        type_stack2, value_stack2 = prg._data_stack.pop()

        if type_stack1 != type_stack2 and type_stack1 != "nil" and type_stack2 != "nil":
            #print("SEM_ERR pop")
            exit(TYPES_ERR)

        prg._data_stack.append(("bool", str(value_stack1 == value_stack2).lower()))

class LtS(Instruction):
    def __init__(self, dict_args):
        super().__init__("LTS", 0, dict_args)

    def check_args(self):
        num = super().get_num_of_arg()

    def execute(self, prg):
        if len(prg._data_stack) < 2:
            #print("ERORO POP")
            exit(NOVALUE_ERR)

        type_stack1, value_stack1 = prg._data_stack.pop()
        type_stack2, value_stack2 = prg._data_stack.pop()

        if type_stack1 != type_stack2:
            #print("SEM_ERR pop")
            exit(TYPES_ERR)

        prg._data_stack.append(("bool", str(value_stack1 < value_stack2).lower()))

class GtS(Instruction):
    def __init__(self, dict_args):
        super().__init__("GTS", 0, dict_args)

    def check_args(self):
        num = super().get_num_of_arg()

    def execute(self, prg):
        if len(prg._data_stack) < 2:
            #print("ERORO POP")
            exit(NOVALUE_ERR)

        type_stack1, value_stack1 = prg._data_stack.pop()
        type_stack2, value_stack2 = prg._data_stack.pop()

        if type_stack1 != type_stack2:
            #print("SEM_ERR pop")
            exit(TYPES_ERR)

        prg._data_stack.append(("bool", str(value_stack1 > value_stack2).lower()))

class Int2charS(Instruction):
    def __init__(self, dict_args):
        super().__init__("INT2CHARS", 0, dict_args)

    def check_args(self):
        num = super().get_num_of_arg()

    def execute(self, prg):
        if len(prg._data_stack) < 1:
            #print("ERORO POP")
            exit(NOVALUE_ERR)

        type_stack1, value_stack1 = prg._data_stack.pop()

        if type_stack1 != "int":
            exit(TYPES_ERR)

        if 0 <= int(value_stack1) <= 1114111:
            prg._data_stack.append(("string", str(chr(int(value_stack1)))))
        else:
            #print("Nejaka chybaaaa")
            exit(STRING_ERR)

class Stri2intS(Instruction):
    def __init__(self, dict_args):
        super().__init__("STRI2INTS", 0, dict_args)

    def check_args(self):
        num = super().get_num_of_arg()

    def execute(self, prg):
        if len(prg._data_stack) < 2:
            #print("ERORO POP")
            exit(NOVALUE_ERR)

        type_stack1, value_stack1 = prg._data_stack.pop()
        type_stack2, value_stack2 = prg._data_stack.pop()

        if type_stack1 != "string" or type_stack2 != "int":
            exit(TYPES_ERR)
        try:
            prg._data_stack.append(("int", str(ord(value_stack1[int(value_stack2)]))))
        except:
            #print("Mimo pole")
            exit(STRING_ERR)

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
                  "SETCHAR" : Setchar, "NOT"         : Not        , "ADDS"     : AddS    ,
                  "CLEARS"  : ClearS , "JUMPIFEQS"   : JumpifeqS  , "SUBS"     : SubS    ,
                  "NOTS"    : NotS   , "JUMPIFNEQS"  : JumpifneqS , "MULS"     : MulS    ,
                  "IDIVS"   : IdivS  , "INT2CHARS"   : Int2charS  , "LTS"      : LtS     ,
                  "GTS"     : GtS    , "STRI2INTS"   : Stri2intS  , "EQS"      : EqS     ,
                  "ANDS"    : AndS   , "ORS"         : OrS
                  }

    @classmethod
    def resolve(cls, string: str, dict_args: dict()):
        try:
            return cls._dict_func[string](dict_args)
        except:
            exit(STRUCT_ERR)
            #print("ERROR with name of instruction, add exit error")

def str2bool(val):
    return True if val == "true" else False;

def where_is_name(name):
    if name == None:
        #print("Where is name")
        exit(UNDECLARE_ERR)

def get_values(instr, id):
    (key, value), = instr.get_arg(id).items()
    frame = value[0:2]
    name, typ, value = instr._get_var(value, prg, key)
    return frame, name, typ, value

def check_type(key1, key2, key3, typ):
    if ((key1 != typ and key1 != None ) or
    key2 != typ or key3 != typ):
        #print("Se")
        exit(TYPES_ERR)

def check_2_keys(key1, typ1, key2, typ2):
    if ((key1 != typ1 and key1 != None) or
    key2 != typ2):
        #print("SEm errrr")
        exit(TYPES_ERR)

def check_label(prg, opcode, dict):
    if opcode.upper() == "LABEL":
        if dict["label"] in prg._labels_dict:
            #print("JOPAAAAAAA")
            exit(SEMANTIC_ERR)

        prg._labels_dict[dict["label"]] = prg.get_num_of_instrs()

def check_order(prg, order, curr_instr):
    try:
        if int(order) < 1:
            #print("ER UUUUU")
            exit(STRUCT_ERR)
    except ValueError:
        exit(STRUCT_ERR)

    if int(order) in prg._order or int(order) < prg._order[curr_instr]:
        exit(STRUCT_ERR)

    prg._order.append(int(order))

def inc_insts(prg, opcode):
    if opcode not in ('LABEL', 'DPRINT', 'BREAK'):
        prg._stats._insts += 1

# MAIN

prg = Program()
cmds = ['help', 'source=', 'input=', 'insts', 'hot', 'vars']
tree = None
try:
    # Define the getopt parameters
    opts, args = getopt.getopt(sys.argv[1:], '', cmds)
    # Check if the options' length is 2 (can be enhanced
      # Iterate the options and get the corresponding values
    for opt, arg in opts:
        if opt in ('--help'):
            if len(sys.argv) != 2:
                exit(PARAM_ERR)
            #print(help)
            exit(0)
        elif opt in ('--source'):
            if tree != None:
                exit(PARAM_ERR)

            try:
                tree = ET.parse(arg)
            except FileNotFoundError:
                exit(PARAM_ERR)
            except ET.ParseError:
                exit(FORMAT_ERR)

        elif opt in ('--input'):
            if prg._input_f != None:
                exit(PARAM_ERR)

            try:
                prg._input_f = open(arg, "r")
            except:
                #print("Nowm file zdaj")
                exit(PARAM_ERR)
        else:
            #print("ERR with argggggg")
            exit(PARAM_ERR)
except getopt.GetoptError:
    # Print something useful
    print ('usage: add.py -a <first_operand> -b <second_operand>')
    exit(2)
        #elif opt in ('--stats'):
        #    if stIsPresent > 0:
        #        self.stats.statsGroups[statsFile] = tmpList
        #    else:
        #        stIsPresent = 1
        #    tmpList = list()
        #    statsFile = arg
        #elif opt in ('--insts', '--hot', '--vars') and stIsPresent > 0:
        #    tmpList.append(opt[2:])
        #else:
        #    exit(ERR_PARAM)

if prg._input_f == None and tree == None:
    exit(PARAM_ERR)
elif prg._input_f == None:
    prg._input_f = None
elif tree == None:
    tree = input()

root = tree.getroot()
if (not re.match('^IPPcode21$', root.attrib['language']) or
    root.tag != "program"):
    exit(STRUCT_ERR)

for child in root:
    #print(type(child.attrib["order"]))
    #print(child.attrib["order"])
    dict_args = dict()
    if (child.tag != "instruction" or "opcode" not in child.attrib or
        "order" not in child.attrib or len(child.attrib) != 2):
        exit(STRUCT_ERR)

    check_order(prg, child.attrib["order"], prg.get_num_of_instrs())
    prg.set_num_of_instrs(prg.get_num_of_instrs() + 1)
    idx = 0

    for ch in child:
        if ch.tag != "arg" + str(idx+1) or idx > 2:
            exit(STRUCT_ERR)

        dict_args[idx] = {ch.attrib["type"] : ch.text}
        idx += 1

    instr = Factory.resolve(child.attrib["opcode"].upper(), dict_args)

    if len(dict_args) != instr.get_num_of_arg():
        exit(STRUCT_ERR)

    if len(dict_args) > 0:
        check_label(prg, child.attrib["opcode"].upper(), dict_args[0])

while prg.get_line() < prg.get_num_of_instrs():
    Instruction.get_instr_list()[prg.get_line()].execute(prg)
    inc_insts(prg, Instruction.get_instr_list()[prg.get_line()]._opcode)
    #print(prg.get_line())
    prg.set_line(prg.get_line() + 1)
