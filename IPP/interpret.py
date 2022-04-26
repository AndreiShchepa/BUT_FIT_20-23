#
# file:   interpret.py
# author: Andrei Shchapaniak <xshcha00>
#

import xml.etree.ElementTree as ET
import sys
import re
import getopt
import collections

# errors
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

class Program:
    def __init__(self):
        self._GF = dict()
        self._LF = None
        self._TF = None
        self._frames = []
        self._labels_dict = dict()
        self._return_lines = []
        self._line = 0
        self._data_stack = []
        self._input_f = None
        self._order = []
        self._insts = 0
        self._file_stats = None
        self._print = []
        self._max_min_op = dict()

class Instruction():
    _instruction_list = []

    def __init__(self, opcode: str, num: int, dict_args):
        self._opcode: str = opcode
        self._num_of_arg: int = num
        self._arguments_dict: dict() = dict_args
        self._instruction_list.append(self)

    def get_arg(self, id):
        return self._arguments_dict[id]

    @classmethod
    def get_instr_list(cls):
        return cls._instruction_list

    def check_frame_none(self, frame):
        if frame == None:
            exit(FRAME_ERR)

    # get info about variable from any frame
    def _get_var(self, var, prg, key):
        if key != "var":
            return None, key, var

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

    # add info about variable to any frame
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

    def execute(self, prg):
        (key, value), = self.get_arg(0).items()
        frame, new_name = value[0:2], value[3:]
        name, typ, value = self._get_var(value, prg, key)

        if name != None or typ != None or value != None:
            exit(SEMANTIC_ERR)

        self._add_var(new_name, frame, None, value, prg)

class Move(Instruction):
    def __init__(self, dict_args):
        super().__init__("MOVE", 2, dict_args)

    def execute(self, prg):
        frame1, name1, typ1, value1 = get_values(self, 0)
        check_declare_var(name1, typ1, value1)
        frame2, name2, typ2, value2 = get_values(self, 1)
        check_declare_var(name2, typ2, value2)

        if value2 == None:
            exit(NOVALUE_ERR)

        self._add_var(name1, frame1, typ2, value2, prg)

class Createframe(Instruction):
    def __init__(self, dict_args):
        super().__init__("CREATEFRAME", 0, dict_args)

    def execute(self, prg):
        prg._TF = dict()

class Popframe(Instruction):
    def __init__(self, dict_args):
        super().__init__("POPFRAME", 0, dict_args)

    def execute(self, prg):
        if len(prg._frames) == 0:
            exit(FRAME_ERR)

        prg._TF = prg._frames.pop()
        prg._LF = prg._frames[-1] if len(prg._frames) > 0 else None

class Break(Instruction):
    def __init__(self, dict_args):
        super().__init__("BREAK", 0, dict_args)

    def execute(self, prg):
        print("Current position: "    + prg._line + 1 + "\n" + \
              "Global frame: "        + prg._GF       + "\n" + \
              "Local frame: "         + prg._LF       + "\n" + \
              "Temporary frame: "     + prg._TF       + "\n" + \
              "Instruction counter: " + prg._stats._insts, file=sys.stderr)

class Call(Instruction):
    def __init__(self, dict_args):
        super().__init__("CALL", 1, dict_args)

    def execute(self, prg):
        (key, value), = self.get_arg(0).items()

        try:
            prg._return_lines.append(prg._line)
            prg._line = prg._labels_dict[value]
        except:
            exit(SEMANTIC_ERR)

class Jump(Instruction):
    def __init__(self, dict_args):
        super().__init__("JUMP", 1, dict_args)

    def execute(self, prg):
        #print(self._arguments_dict[0])
        (key, value), = self.get_arg(0).items()
        #print(prg._labels_dict)

        try:
            prg._line = prg._labels_dict[value]
        except:
            exit(SEMANTIC_ERR)

class Exit(Instruction):
    def __init__(self, dict_args):
        super().__init__("EXIT", 1, dict_args)

    def check_semantic(self, typ1, value1, typ2, value2, typ3, value3):
        if value1 == None:
            exit(NOVALUE_ERR)
        if typ1 != "int":
            exit(TYPES_ERR)
        if int(value1) > 49 or int(value1) < 0:
            exit(WRONG_VALUE_ERR)

    def execute(self, prg):
        (key, value), = self.get_arg(0).items()
        frame = value[0:2] if key == "var" else None
        name, typ, value = self._get_var(value, prg, key)
        check_declare_var(name, typ, value)

        self.check_semantic(typ, value, None, None, None, None)

        if prg._file_stats != None:
            for i in range (len(prg._print)):
                print(prg._insts if prg._print[i] == 'i' else get_max_min(prg._max_min_op), file=prg._file_stats)

        exit(int(value))

class Jumpifeq(Instruction):
    def __init__(self, dict_args):
        super().__init__("JUMPIFEQ", 3, dict_args)

    def check_semantic(self, typ1, value1, typ2, value2, typ3, value3):
        if value2 == None or value3 == None:
            exit(NOVALUE_ERR)
        if typ2 != typ3 and typ2 != "nil" and typ3 != "nil":
            exit(TYPES_ERR)

    def execute(self, prg):
        (key, value), = self.get_arg(0).items()
        frame2, name2, typ2, value2 = get_values(self, 1)
        check_declare_var(name2, typ2, value2)
        frame3, name3, typ3, value3 = get_values(self, 2)
        check_declare_var(name3, typ3, value3)

        self.check_semantic(None, None, typ2, value2, typ3, value3)
        if value in prg._labels_dict:
            if value2 == value3 :
                prg._line = prg._labels_dict[value]
        else:
            exit(SEMANTIC_ERR)

class Add(Instruction):
    def __init__(self, dict_args):
        super().__init__("ADD", 3, dict_args)

    def check_semantic(self, typ1, value1, typ2, value2, typ3, value3):
        if value2 == None or value3 == None:
            exit(NOVALUE_ERR)
        if typ2 != "int" or typ3 != "int":
            exit(TYPES_ERR)

    def execute(self, prg):
        frame1, name1, typ1, value1 = get_values(self, 0)
        check_declare_var(name1, typ1, value1)
        frame2, name2, typ2, value2 = get_values(self, 1)
        check_declare_var(name2, typ2, value2)
        frame3, name3, typ3, value3 = get_values(self, 2)
        check_declare_var(name3, typ3, value3)

        self.check_semantic(typ1, value1, typ2, value2, typ3, value3)
        try:
            self._add_var(name1, frame1, "int", str(int(value2) + int(value3)), prg)
        except ValueError:
            exit(STRUCT_ERR)

class Mul(Instruction):
    def __init__(self, dict_args):
        super().__init__("MUL", 3, dict_args)

    def check_semantic(self, typ1, value1, typ2, value2, typ3, value3):
        if value2 == None or value3 == None:
            exit(NOVALUE_ERR)
        if typ2 != "int" or typ3 != "int":
            exit(TYPES_ERR)

    def execute(self, prg):
        frame1, name1, typ1, value1 = get_values(self, 0)
        check_declare_var(name1, typ1, value1)
        frame2, name2, typ2, value2 = get_values(self, 1)
        check_declare_var(name2, typ2, value2)
        frame3, name3, typ3, value3 = get_values(self, 2)
        check_declare_var(name3, typ3, value3)

        self.check_semantic(typ1, value1, typ2, value2, typ3, value3)
        try:
            self._add_var(name1, frame1, "int", str(int(value2) * int(value3)), prg)
        except ValueError:
            exit(STRUCT_ERR)

class Lt(Instruction):
    def __init__(self, dict_args):
        super().__init__("LT", 3, dict_args)

    def check_semantic(self, typ1, value1, typ2, value2, typ3, value3):
        if value2 == None or value3 == None:
            exit(NOVALUE_ERR)
        if typ2 == "nil" or typ3 == "nil":
            exit(TYPES_ERR)
        if typ2 != typ3:
            exit(TYPES_ERR)

    def execute(self, prg):
        frame1, name1, typ1, value1 = get_values(self, 0)
        check_declare_var(name1, typ1, value1)
        frame2, name2, typ2, value2 = get_values(self, 1)
        check_declare_var(name2, typ2, value2)
        frame3, name3, typ3, value3 = get_values(self, 2)
        check_declare_var(name3, typ3, value3)

        self.check_semantic(typ1, value1, typ2, value2, typ3, value3)
        if typ2 == "int":
            self._add_var(name1, frame1, "bool", str(int(value2) < int(value3)).lower(), prg)
        elif typ2 == "string":
            self._add_var(name1, frame1, "bool", str(value2 < value3).lower(), prg)
        elif typ2 == "bool" and value2 == "false" and value3 == "true":
            self._add_var(name1, frame1, "bool", "true", prg)
        else:
            self._add_var(name1, frame1, "bool", "false", prg)

class Eq(Instruction):
    def __init__(self, dict_args):
        super().__init__("EQ", 3, dict_args)

    def check_semantic(self, typ1, value1, typ2, value2, typ3, value3):
        if value2 == None or value3 == None:
            exit(NOVALUE_ERR)
        if typ2 != typ3 and typ2 != "nil" and typ3 != "nil":
            exit(TYPES_ERR)

    def execute(self, prg):
        frame1, name1, typ1, value1 = get_values(self, 0)
        check_declare_var(name1, typ1, value1)
        frame2, name2, typ2, value2 = get_values(self, 1)
        check_declare_var(name2, typ2, value2)
        frame3, name3, typ3, value3 = get_values(self, 2)
        check_declare_var(name3, typ3, value3)

        self.check_semantic(typ1, value1, typ2, value2, typ3, value3)
        self._add_var(name1, frame1, "bool", str(value2 == value3).lower(), prg)

class Or(Instruction):
    def __init__(self, dict_args):
        super().__init__("OR", 3, dict_args)

    def check_semantic(self, typ1, value1, typ2, value2, typ3, value3):
        if value2 == None or value3 == None:
            exit(NOVALUE_ERR)
        if typ2 != "bool" or typ3 != "bool":
            exit(TYPES_ERR)

    def execute(self, prg):
        frame1, name1, typ1, value1 = get_values(self, 0)
        check_declare_var(name1, typ1, value1)
        frame2, name2, typ2, value2 = get_values(self, 1)
        check_declare_var(name2, typ2, value2)
        frame3, name3, typ3, value3 = get_values(self, 2)
        check_declare_var(name3, typ3, value3)

        self.check_semantic(typ1, value1, typ2, value2, typ3, value3)
        self._add_var(name1, frame1, "bool", str(str2bool(value2) or str2bool(value3)).lower(), prg)

class Concat(Instruction):
    def __init__(self, dict_args):
        super().__init__("CONCAT", 3, dict_args)

    def check_semantic(self, typ1, value1, typ2, value2, typ3, value3):
        if value2 == None or value3 == None:
            exit(NOVALUE_ERR)
        if typ2 != "string" or typ3 != "string":
            exit(TYPES_ERR)

    def execute(self, prg):
        frame1, name1, typ1, value1 = get_values(self, 0)
        check_declare_var(name1, typ1, value1)
        frame2, name2, typ2, value2 = get_values(self, 1)
        check_declare_var(name2, typ2, value2)
        frame3, name3, typ3, value3 = get_values(self, 2)
        check_declare_var(name3, typ3, value3)

        self.check_semantic(typ1, value1, typ2, value2, typ3, value3)
        self._add_var(name1, frame1, "string", value2 + value3, prg)

class Setchar(Instruction):
    def __init__(self, dict_args):
        super().__init__("SETCHAR", 3, dict_args)

    def check_semantic(self, typ1, value1, typ2, value2, typ3, value3):
        if value1 == None or value2 == None or value3 == None:
            exit(NOVALUE_ERR)
        if typ2 != "int" or typ3 != "string" or typ1 != "string":
            exit(TYPES_ERR)
        if int(value2) < 0 or int(value2) + 1 > len(value1):
            exit(STRING_ERR)

    def execute(self, prg):
        frame1, name1, typ1, value1 = get_values(self, 0)
        check_declare_var(name1, typ1, value1)
        frame2, name2, typ2, value2 = get_values(self, 1)
        check_declare_var(name2, typ2, value2)
        frame3, name3, typ3, value3 = get_values(self, 2)
        check_declare_var(name3, typ3, value3)

        self.check_semantic(typ1, value1, typ2, value2, typ3, value3)
        try:
            value1 = value1[:int(value2)] + value3[0] + value1[int(value2)+1:]
            self._add_var(name1, frame1, "string", value1, prg)
        except:
            exit(STRING_ERR)

class Stri2int(Instruction):
    def __init__(self, dict_args):
        super().__init__("STRI2INT", 3, dict_args)

    def check_semantic(self, typ1, value1, typ2, value2, typ3, value3):
        if value2 == None or value3 == None:
            exit(NOVALUE_ERR)
        if typ3 != "int" or typ2 != "string":
            exit(TYPES_ERR)
        if int(value3) < 0 or int(value3) + 1 > len(value2):
            exit(STRING_ERR)

    def execute(self, prg):
        frame1, name1, typ1, value1 = get_values(self, 0)
        check_declare_var(name1, typ1, value1)
        frame2, name2, typ2, value2 = get_values(self, 1)
        check_declare_var(name2, typ2, value2)
        frame3, name3, typ3, value3 = get_values(self, 2)
        check_declare_var(name3, typ3, value3)

        self.check_semantic(typ1, value1, typ2, value2, typ3, value3)
        try:
            self._add_var(name1, frame1, "int", str(ord(value2[int(value3)])), prg)
        except:
            exit(STRING_ERR)

class Pushframe(Instruction):
    def __init__(self, dict_args):
        super().__init__("PUSHFRAME", 0, dict_args)

    def execute(self, prg):
        if prg._TF is None:
            exit(FRAME_ERR)

        prg._frames.append(prg._TF)
        prg._LF = prg._TF
        prg._TF = None

class Return(Instruction):
    def __init__(self, dict_args):
        super().__init__("Return", 0, dict_args)

    def execute(self, prg):
        if len(prg._return_lines) == 0:
            exit(NOVALUE_ERR)

        prg._line = prg._return_lines.pop()

class Strlen(Instruction):
    def __init__(self, dict_args):
        super().__init__("STRLEN", 2, dict_args)

    def check_semantic(self, typ1, value1, typ2, value2, typ3, value3):
        if value2 == None:
            exit(NOVALUE_ERR)
        if typ2 != "string":
            exit(TYPES_ERR)

    def execute(self, prg):
        frame1, name1, typ1, value1 = get_values(self, 0)
        check_declare_var(name1, typ1, value1)
        frame2, name2, typ2, value2 = get_values(self, 1)
        check_declare_var(name2, typ2, value2)

        self.check_semantic(typ1, value1, typ2, value2, None, None)
        self._add_var(name1, frame1, "int", len(value2), prg)

class Label(Instruction):
    def __init__(self, dict_args):
        super().__init__("LABEL", 1, dict_args)

    def execute(self, prg):
        pass

class Type(Instruction):
    def __init__(self, dict_args):
        super().__init__("TYPE", 2, dict_args)

    def execute(self, prg):
        frame1, name1, typ1, value1 = get_values(self, 0)
        check_declare_var(name1, typ1, value1)
        frame2, name2, typ2, value2 = get_values(self, 1)
        check_declare_var(name2, typ2, value2)

        # if value2 == None:
        #     exit(NOVALUE_ERR)

        typ2 = "" if typ2 == None else typ2
        self._add_var(name1, frame1, "string", typ2, prg)

class Write(Instruction):
    def __init__(self, dict_args):
        super().__init__("WRITE", 1, dict_args)

    def execute(self, prg):
        frame, name, typ, value = get_values(self, 0)
        check_declare_var(name, typ, value)

        if value == None:
            exit(NOVALUE_ERR)
        else:
            print("" if typ == "nil" else value, end='', file=sys.stdout)

class Dprint(Instruction):
    def __init__(self, dict_args):
        super().__init__("DPRINT", 1, dict_args)

    def execute(self, prg):
        frame, name, typ, value = get_values(self, 0)
        check_declare_var(name, typ, value)

        if value == None:
            exit(NOVALUE_ERR)
        else:
            print("" if typ == "nil" else value, end='', file=sys.stderr)

class Int2char(Instruction):
    def __init__(self, dict_args):
        super().__init__("INT2CHAR", 2, dict_args)

    def check_semantic(self, typ1, value1, typ2, value2, typ3, value3):
        if value2 == None:
            exit(NOVALUE_ERR)
        if typ2 != "int":
            exit(TYPES_ERR)
        if 0 > int(value2) or int(value2) > 1114111:
            exit(STRING_ERR)

    def execute(self, prg):
        frame1, name1, typ1, value1 = get_values(self, 0)
        check_declare_var(name1, typ1, value1)
        frame2, name2, typ2, value2 = get_values(self, 1)
        check_declare_var(name2, typ2, value2)

        self.check_semantic(typ1, value1, typ2, value2, None, None)
        self._add_var(name1, frame1, "string", str(chr(int(value2))), prg)

class Jumpifneq(Instruction):
    def __init__(self, dict_args):
        super().__init__("JUMPIFNEQ", 3, dict_args)

    def check_semantic(self, typ1, value1, typ2, value2, typ3, value3):
        if value2 == None or value3 == None:
            exit(NOVALUE_ERR)
        if typ2 != typ3 and typ2 != "nil" and typ3 != "nil":
            exit(TYPES_ERR)

    def execute(self, prg):
        (key, value), = self.get_arg(0).items()
        frame2, name2, typ2, value2 = get_values(self, 1)
        check_declare_var(name2, typ2, value2)
        frame3, name3, typ3, value3 = get_values(self, 2)
        check_declare_var(name3, typ3, value3)

        self.check_semantic(None, None, typ2, value2, typ3, value3)
        if value in prg._labels_dict:
            if value2 != value3 :
                prg._line = prg._labels_dict[value]
        else:
            exit(SEMANTIC_ERR)

class Sub(Instruction):
    def __init__(self, dict_args):
        super().__init__("SUB", 3, dict_args)

    def check_semantic(self, typ1, value1, typ2, value2, typ3, value3):
        if value2 == None or value3 == None:
            exit(NOVALUE_ERR)
        if typ2 != "int" or typ3 != "int":
            exit(TYPES_ERR)

    def execute(self, prg):
        frame1, name1, typ1, value1 = get_values(self, 0)
        check_declare_var(name1, typ1, value1)
        frame2, name2, typ2, value2 = get_values(self, 1)
        check_declare_var(name2, typ2, value2)
        frame3, name3, typ3, value3 = get_values(self, 2)
        check_declare_var(name3, typ3, value3)

        self.check_semantic(typ1, value1, typ2, value2, typ3, value3)
        try:
            self._add_var(name1, frame1, "int", str(int(value2) - int(value3)), prg)
        except ValueError:
            exit(STRUCT_ERR)

class Idiv(Instruction):
    def __init__(self, dict_args):
        super().__init__("IDIV", 3, dict_args)

    def check_semantic(self, typ1, value1, typ2, value2, typ3, value3):
        if value2 == None or value3 == None:
            exit(NOVALUE_ERR)
        if typ2 != "int" or typ3 != "int":
            exit(TYPES_ERR)
        if int(value3) == 0:
            exit(WRONG_VALUE_ERR)

    def execute(self, prg):
        frame1, name1, typ1, value1 = get_values(self, 0)
        check_declare_var(name1, typ1, value1)
        frame2, name2, typ2, value2 = get_values(self, 1)
        check_declare_var(name2, typ2, value2)
        frame3, name3, typ3, value3 = get_values(self, 2)
        check_declare_var(name3, typ3, value3)

        self.check_semantic(typ1, value1, typ2, value2, typ3, value3)
        try:
            self._add_var(name1, frame1, "int", str(int(value2) // int(value3)), prg)
        except ValueError:
            exit(STRUCT_ERR)

class Gt(Instruction):
    def __init__(self, dict_args):
        super().__init__("GT", 3, dict_args)

    def check_semantic(self, typ1, value1, typ2, value2, typ3, value3):
        if value2 == None or value3 == None:
            exit(NOVALUE_ERR)
        if typ2 == "nil" or typ3 == "nil":
            exit(TYPES_ERR)
        if typ2 != typ3:
            exit(TYPES_ERR)

    def execute(self, prg):
        frame1, name1, typ1, value1 = get_values(self, 0)
        check_declare_var(name1, typ1, value1)
        frame2, name2, typ2, value2 = get_values(self, 1)
        check_declare_var(name2, typ2, value2)
        frame3, name3, typ3, value3 = get_values(self, 2)
        check_declare_var(name3, typ3, value3)

        self.check_semantic(typ1, value1, typ2, value2, typ3, value3)
        if typ2 == "int":
            self._add_var(name1, frame1, "bool", str(int(value2) > int(value3)).lower(), prg)
        elif typ2 == "string":
            self._add_var(name1, frame1, "bool", str(value2 > value3).lower(), prg)
        elif typ2 == "bool" and value2 == "true" and value3 == "false":
            self._add_var(name1, frame1, "bool", "true", prg)
        else:
            self._add_var(name1, frame1, "bool", "false", prg)

class And(Instruction):
    def __init__(self, dict_args):
        super().__init__("AND", 3, dict_args)

    def check_semantic(self, typ1, value1, typ2, value2, typ3, value3):
        if value2 == None or value3 == None:
            exit(NOVALUE_ERR)
        if typ2 != "bool" or typ3 != "bool":
            exit(TYPES_ERR)

    def execute(self, prg):
        frame1, name1, typ1, value1 = get_values(self, 0)
        check_declare_var(name1, typ1, value1)
        frame2, name2, typ2, value2 = get_values(self, 1)
        check_declare_var(name2, typ2, value2)
        frame3, name3, typ3, value3 = get_values(self, 2)
        check_declare_var(name3, typ3, value3)

        self.check_semantic(typ1, value1, typ2, value2, typ3, value3)
        self._add_var(name1, frame1, "bool", str(str2bool(value2) and str2bool(value3)).lower(), prg)

class Not(Instruction):
    def __init__(self, dict_args):
        super().__init__("NOT", 2, dict_args)

    def check_semantic(self, typ1, value1, typ2, value2, typ3, value3):
        if value2 == None:
            exit(NOVALUE_ERR)
        if typ2 != "bool":
            exit(TYPES_ERR)

    def execute(self, prg):
        frame1, name1, typ1, value1 = get_values(self, 0)
        check_declare_var(name1, typ1, value1)
        frame2, name2, typ2, value2 = get_values(self, 1)
        check_declare_var(name2, typ2, value2)

        self.check_semantic(typ1, value1, typ2, value2, None, None)
        self._add_var(name1, frame1, "bool", str(not str2bool(value2)).lower(), prg)

class Getchar(Instruction):
    def __init__(self, dict_args):
        super().__init__("GETCHAR", 3, dict_args)

    def check_semantic(self, typ1, value1, typ2, value2, typ3, value3):
        if value2 == None or value3 == None:
            exit(NOVALUE_ERR)
        if typ3 != "int" or typ2 != "string":
            exit(TYPES_ERR)
        if int(value3) < 0:
            exit(STRING_ERR)

    def execute(self, prg):
        frame1, name1, typ1, value1 = get_values(self, 0)
        check_declare_var(name1, typ1, value1)
        frame2, name2, typ2, value2 = get_values(self, 1)
        check_declare_var(name2, typ2, value2)
        frame3, name3, typ3, value3 = get_values(self, 2)
        check_declare_var(name3, typ3, value3)

        self.check_semantic(typ1, value1, typ2, value2, typ3, value3)
        try:
            self._add_var(name1, frame1, "string", str(value2[int(value3)]), prg)
        except:
            exit(STRING_ERR)

class Read(Instruction):
    def __init__(self, dict_args):
        super().__init__("READ", 2, dict_args)

    def execute(self, prg):
        (key, value), = self.get_arg(1).items()
        frame1, name1, typ1, value1 = get_values(self, 0)
        check_declare_var(name1, typ1, value1)

        try:
            line = input() if prg._input_f == None else prg._input_f.readline()
            if line == "":
                self._add_var(name1, frame1, "nil", "nil", prg)
                return

            line = line.rstrip()

            if value == 'int':
                try:
                    self._add_var(name1, frame1, "int", str(int(line)), prg)
                except:
                    self._add_var(name1, frame1, "nil", "nil", prg)
            elif value == 'string':
                self._add_var(name1, frame1, "string", str(line), prg)
            elif value == 'bool':
                new_v = "true" if line.upper() == "TRUE" else "false"
                self._add_var(name1, frame1, "bool", new_v, prg)
        except EOFError:
            self._add_var(name1, frame1, "nil", "nil", prg)

class PushS(Instruction):
    def __init__(self, dict_args):
        super().__init__("PUSHS", 1, dict_args)

    def execute(self, prg):
        (key, value), = self.get_arg(0).items()
        frame = value[0:2] if key == "var" else None
        name, typ, value = self._get_var(value, prg, key)
        check_declare_var(name, typ, value)

        if value == None:
            exit(NOVALUE_ERR)
        else:
            prg._data_stack.append((typ, value))

class PopS(Instruction):
    def __init__(self, dict_args):
        super().__init__("POPS", 1, dict_args)

    def execute(self, prg):
        if len(prg._data_stack) == 0:
            exit(NOVALUE_ERR)

        frame, name, typ, value = get_values(self, 0)
        type_stack, value_stack = prg._data_stack.pop()
        check_declare_var(name, typ, value)

        self._add_var(name, frame, type_stack, value_stack, prg)

class AddS(Instruction):
    def __init__(self, dict_args):
        super().__init__("ADDS", 0, dict_args)

    def check_semantic(self, value1, typ1, value2, typ2):
        if value2 == None or value1 == None:
            exit(NOVALUE_ERR)
        if typ2 != "int" or typ1 != "int":
            exit(TYPES_ERR)

    def execute(self, prg):
        if len(prg._data_stack) < 2:
            exit(NOVALUE_ERR)

        type_stack1, value_stack1 = prg._data_stack.pop()
        type_stack2, value_stack2 = prg._data_stack.pop()

        self.check_semantic(value_stack1, type_stack1, value_stack2, type_stack2)
        prg._data_stack.append((type_stack1, int(value_stack1) + int(value_stack2)))

class SubS(Instruction):
    def __init__(self, dict_args):
        super().__init__("SUBS", 0, dict_args)

    def check_semantic(self, value1, typ1, value2, typ2):
        if value2 == None or value1 == None:
            exit(NOVALUE_ERR)
        if typ2 != "int" or typ1 != "int":
            exit(TYPES_ERR)

    def execute(self, prg):
        if len(prg._data_stack) < 2:
            exit(NOVALUE_ERR)

        type_stack1, value_stack1 = prg._data_stack.pop()
        type_stack2, value_stack2 = prg._data_stack.pop()

        self.check_semantic(value_stack1, type_stack1, value_stack2, type_stack2)
        prg._data_stack.append((type_stack1, int(value_stack2) - int(value_stack1)))

class MulS(Instruction):
    def __init__(self, dict_args):
        super().__init__("MULS", 0, dict_args)

    def check_semantic(self, value1, typ1, value2, typ2):
        if value2 == None or value1 == None:
            exit(NOVALUE_ERR)
        if typ2 != "int" or typ1 != "int":
            exit(TYPES_ERR)

    def execute(self, prg):
        if len(prg._data_stack) < 2:
            exit(NOVALUE_ERR)

        type_stack1, value_stack1 = prg._data_stack.pop()
        type_stack2, value_stack2 = prg._data_stack.pop()

        self.check_semantic(value_stack1, type_stack1, value_stack2, type_stack2)
        prg._data_stack.append((type_stack1, int(value_stack1) * int(value_stack2)))

class IdivS(Instruction):
    def __init__(self, dict_args):
        super().__init__("IDIVS", 0, dict_args)

    def check_semantic(self, value1, typ1, value2, typ2):
        if value2 == None or value1 == None:
            exit(NOVALUE_ERR)
        if typ2 != "int" or typ1 != "int":
            exit(TYPES_ERR)
        if int(value2) == 0:
            exit(WRONG_VALUE_ERR)

    def execute(self, prg):
        if len(prg._data_stack) < 2:
            exit(NOVALUE_ERR)

        type_stack1, value_stack1 = prg._data_stack.pop()
        type_stack2, value_stack2 = prg._data_stack.pop()

        self.check_semantic(value_stack1, type_stack1, value_stack2, type_stack2)
        prg._data_stack.append((type_stack1, int(value_stack2) // int(value_stack1)))

class ClearS(Instruction):
    def __init__(self, dict_args):
        super().__init__("CLEARS", 0, dict_args)

    def execute(self, prg):
        prg._data_stack = []

class JumpifeqS(Instruction):
    def __init__(self, dict_args):
        super().__init__("JUMPIFEQS", 1, dict_args)

    def check_semantic(self, value1, typ1, value2, typ2):
        if value1 == None or value2 == None:
            exit(NOVALUE_ERR)
        if typ1 != typ2 and typ2 != "nil" and typ1 != "nil":
            exit(TYPES_ERR)

    def execute(self, prg):
        (key, value), = self.get_arg(0).items()
        type_stack1, value_stack1 = prg._data_stack.pop()
        type_stack2, value_stack2 = prg._data_stack.pop()

        self.check_semantic(value_stack1, type_stack1, value_stack2, type_stack2)
        if value in prg._labels_dict:
            if value_stack1 == value_stack2 :
                prg._line = prg._labels_dict[value]
        else:
            exit(SEMANTIC_ERR)

class JumpifneqS(Instruction):
    def __init__(self, dict_args):
        super().__init__("JUMPIFNEQS", 1, dict_args)

    def check_semantic(self, value1, typ1, value2, typ2):
        if value1 == None or value2 == None:
            exit(NOVALUE_ERR)
        if typ1 != typ2 and typ2 != "nil" and typ1 != "nil":
            exit(TYPES_ERR)

    def execute(self, prg):
        (key, value), = self.get_arg(0).items()
        type_stack1, value_stack1 = prg._data_stack.pop()
        type_stack2, value_stack2 = prg._data_stack.pop()

        self.check_semantic(value_stack1, type_stack1, value_stack2, type_stack2)
        if value in prg._labels_dict:
            if value_stack1 != value_stack2 :
                prg._line = prg._labels_dict[value]
        else:
            exit(SEMANTIC_ERR)

class NotS(Instruction):
    def __init__(self, dict_args):
        super().__init__("NOTS", 0, dict_args)

    def check_semantic(self, value1, typ1):
        if value1 == None:
            exit(NOVALUE_ERR)
        if typ1 == "nil":
            exit(TYPES_ERR)

    def execute(self, prg):
        if len(prg._data_stack) < 1:
            exit(NOVALUE_ERR)

        type_stack1, value_stack1 = prg._data_stack.pop()

        self.check_semantic(value_stack1, type_stack1)
        prg._data_stack.append(("bool", str(not str2bool(value_stack1)).lower()))

class AndS(Instruction):
    def __init__(self, dict_args):
        super().__init__("ANDS", 0, dict_args)

    def check_semantic(self, value1, typ1, value2, typ2):
        if value2 == None or value1 == None:
            exit(NOVALUE_ERR)
        if typ2 != "bool" or typ1 != "bool":
            exit(TYPES_ERR)

    def execute(self, prg):
        if len(prg._data_stack) < 2:
            exit(NOVALUE_ERR)

        type_stack1, value_stack1 = prg._data_stack.pop()
        type_stack2, value_stack2 = prg._data_stack.pop()

        self.check_semantic(value_stack1, type_stack1, value_stack2, type_stack2)
        prg._data_stack.append(("bool", str(str2bool(value_stack1) and str2bool(value_stack2)).lower()))

class OrS(Instruction):
    def __init__(self, dict_args):
        super().__init__("ORS", 0, dict_args)

    def check_semantic(self, value1, typ1, value2, typ2):
        if value2 == None or value1 == None:
            exit(NOVALUE_ERR)
        if typ2 != "bool" or typ1 != "bool":
            exit(TYPES_ERR)

    def execute(self, prg):
        if len(prg._data_stack) < 2:
            exit(NOVALUE_ERR)

        type_stack1, value_stack1 = prg._data_stack.pop()
        type_stack2, value_stack2 = prg._data_stack.pop()

        self.check_semantic(value_stack1, type_stack1, value_stack2, type_stack2)
        prg._data_stack.append(("bool", str(str2bool(value_stack1) or str2bool(value_stack2)).lower()))

class EqS(Instruction):
    def __init__(self, dict_args):
        super().__init__("EQS", 0, dict_args)

    def check_semantic(self, value1, typ1, value2, typ2):
        if value2 == None or value1 == None:
            exit(NOVALUE_ERR)
        if typ2 != typ1 and typ2 != "nil" and typ1 != "nil":
            exit(TYPES_ERR)

    def execute(self, prg):
        if len(prg._data_stack) < 2:
            exit(NOVALUE_ERR)

        type_stack1, value_stack1 = prg._data_stack.pop()
        type_stack2, value_stack2 = prg._data_stack.pop()

        self.check_semantic(value_stack1, type_stack1, value_stack2, type_stack2)
        prg._data_stack.append(("bool", str(value_stack1 == value_stack2).lower()))

class LtS(Instruction):
    def __init__(self, dict_args):
        super().__init__("LTS", 0, dict_args)

    def check_semantic(self, value1, typ1, value2, typ2):
        if value2 == None or value1 == None:
            exit(NOVALUE_ERR)
        if typ2 == "nil" or typ1 == "nil":
            exit(TYPES_ERR)
        if typ2 != typ1:
            exit(TYPES_ERR)

    def execute(self, prg):
        if len(prg._data_stack) < 2:
            exit(NOVALUE_ERR)

        type_stack1, value_stack1 = prg._data_stack.pop()
        type_stack2, value_stack2 = prg._data_stack.pop()

        self.check_semantic(value_stack1, type_stack1, value_stack2, type_stack2)
        if type_stack2 == "int":
            prg._data_stack.append(("bool", str(int(value_stack2) < int(value_stack1)).lower()))
        elif type_stack2 == "string":
            prg._data_stack.append(("bool", str(value_stack2 < value_stack1).lower()))
        elif type_stack2 == "bool" and value_stack2 == "false" and value_stack1 == "true":
            prg._data_stack.append(("bool", "true"))
        else:
            prg._data_stack.append(("bool", "false"))

class GtS(Instruction):
    def __init__(self, dict_args):
        super().__init__("GTS", 0, dict_args)

    def check_semantic(self, value1, typ1, value2, typ2):
        if value2 == None or value1 == None:
            exit(NOVALUE_ERR)
        if typ2 == "nil" or typ1 == "nil":
            exit(TYPES_ERR)
        if typ2 != typ1:
            exit(TYPES_ERR)

    def execute(self, prg):
        if len(prg._data_stack) < 2:
            exit(NOVALUE_ERR)

        type_stack1, value_stack1 = prg._data_stack.pop()
        type_stack2, value_stack2 = prg._data_stack.pop()

        self.check_semantic(value_stack1, type_stack1, value_stack2, type_stack2)
        if type_stack2 == "int":
            prg._data_stack.append(("bool", str(int(value_stack2) > int(value_stack1)).lower()))
        elif type_stack2 == "string":
            prg._data_stack.append(("bool", str(value_stack2 > value_stack1).lower()))
        elif type_stack2 == "bool" and value_stack1 == "false" and value_stack2 == "true":
            prg._data_stack.append(("bool", "true"))
        else:
            prg._data_stack.append(("bool", "false"))

class Int2charS(Instruction):
    def __init__(self, dict_args):
        super().__init__("INT2CHARS", 0, dict_args)

    def check_semantic(self, value1, typ1, value2, typ2):
        if value1 == None:
            exit(NOVALUE_ERR)
        if typ1 != "int":
            exit(TYPES_ERR)
        if 0 > int(value1) or int(value1) > 1114111:
            exit(STRING_ERR)

    def execute(self, prg):
        if len(prg._data_stack) < 1:
            exit(NOVALUE_ERR)

        type_stack1, value_stack1 = prg._data_stack.pop()

        self.check_semantic(value_stack1, type_stack1, None, None)
        prg._data_stack.append(("string", str(chr(int(value_stack1)))))

class Stri2intS(Instruction):
    def __init__(self, dict_args):
        super().__init__("STRI2INTS", 0, dict_args)

    def check_semantic(self, value1, typ1, value2, typ2):
        if value2 == None or value1 == None:
            exit(NOVALUE_ERR)
        if typ1 != "int" or typ2 != "string":
            exit(TYPES_ERR)
        if int(value1) < 0 or int(value1) + 1 > len(value2):
            exit(STRING_ERR)

    def execute(self, prg):
        if len(prg._data_stack) < 2:
            exit(NOVALUE_ERR)

        type_stack1, value_stack1 = prg._data_stack.pop()
        type_stack2, value_stack2 = prg._data_stack.pop()

        self.check_semantic(value_stack1, type_stack1, value_stack2, type_stack2)
        prg._data_stack.append(("int", str(ord(value_stack2[int(value_stack1)]))))

class Factory:
    _dict_func = {"DEFVAR"  : Defvar , "CREATEFRAME" : Createframe, "POPFRAME" : Popframe,
                  "BREAK"   : Break  , "CALL"        : Call       , "JUMP"     : Jump    ,
                  "EXIT"    : Exit   , "MOVE"        : Move       , "ADD"      : Add     ,
                  "CONCAT"  : Concat , "STRI2INT"    : Stri2int   , "LT"       : Lt      ,
                  "RETURN"  : Return , "INT2CHAR"    : Int2char   , "EQ"       : Eq      ,
                  "STRLEN"  : Strlen , "PUSHFRAME"   : Pushframe  , "OR"       : Or      ,
                  "DPRINT"  : Dprint , "JUMPIFNEQ"   : Jumpifneq  , "POPS"     : PopS    ,
                  "GETCHAR" : Getchar, "JUMPIFEQ"    : Jumpifeq   , "LABEL"    : Label   ,
                  "WRITE"   : Write  , "TYPE"        : Type       , "IDIV"     : Idiv    ,
                  "READ"    : Read   , "AND"         : And        , "GT"       : Gt      ,
                  "PUSHS"   : PushS  , "MUL"         : Mul        , "SUB"      : Sub     ,
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

# convert string vaue to bool
def str2bool(val):
    return True if val == "true" else False

# if all name, typ, value are None,
# variable is not declare in any frame
def check_declare_var(name, typ, value):
    if name == None and typ == None and typ == None:
        exit(UNDECLARE_ERR)

# get type, value and name of desired variable
# if variable of type string is None, it is an empty string
# only variables have frame, symbols have bot
def get_values(instr, id):
    (key, value), = instr.get_arg(id).items()
    value = "" if key == "string" and value == None else value
    frame = value[0:2] if key == "var" else None
    name, typ, value = instr._get_var(value, prg, key)
    return frame, name, typ, value

# values must have the same type to perform operations with them
# the first (assigning) variable may has no type
def check_type(key1, key2, key3, typ):
    if ((key1 != typ and key1 != None ) or key2 != typ or key3 != typ):
        exit(TYPES_ERR)

# the same function as function above, but check 2 values instead of 3
def check_2_keys(key1, typ1, key2, typ2):
    if ((key1 != typ1 and key1 != None) or key2 != typ2):
        exit(TYPES_ERR)

# check unique labels and add to dictionary with "line" where it occured
def check_label(prg, dict_instr):
    i = -1
    for item in dict_instr:
        i += 1
        if item[1]._opcode.upper() == "LABEL":
            label_name = item[1]._arguments_dict[0]['label']
            if label_name in prg._labels_dict:
                exit(SEMANTIC_ERR)

            prg._labels_dict[label_name] = i

# find instruction which was performed the most times
def get_max_min(dict_my):
    arr = list(dict_my.values())
    max = 0
    ret_order = 0

    for i in range(len(arr)):
        if arr[i][1] > max:
            ret_order = arr[i][0]
            max = arr[i][1]

    return ret_order

# check positive unique order
def check_order(prg, order):
    try:
        if int(order) < 1:
            exit(STRUCT_ERR)
    except ValueError:
        exit(STRUCT_ERR)

    if int(order) in prg._order:
        exit(STRUCT_ERR)

    prg._order.append(int(order))

# count instructions for statistics
def inc_insts(prg, opcode, order):
    if opcode not in ('LABEL', 'DPRINT', 'BREAK'):
        prg._insts += 1

        if opcode not in prg._max_min_op.keys():
            prg._max_min_op[opcode] = [order, 1]
        else:
            prg._max_min_op[opcode][1] += 1

# print help message
def help_msg():
    if len(sys.argv) != 2:
        exit(PARAM_ERR)

    print("Usage: python3.8 interpret.py [--help] [--stats=file] [--source=file] [--input=file] [--insts]")
    print("\t--help prints help to standard output.")
    print("\t--stats=file prints statistics to the file.")
    print("\t--source=file file with source XML code.")
    print("\t--input=file file with possible input for instruction READ.")
    print("\t--insts prints the number of so-called executed instructions.")
    exit(0)

def read_xml(tree, arg):
    if tree != None:
        exit(PARAM_ERR)

    try:
        return ET.parse(arg)
    except FileNotFoundError:
        exit(PARAM_ERR)
    except ET.ParseError:
        exit(FORMAT_ERR)

def open_file(prg, arg, mode, file):
    if file != None:
        exit(PARAM_ERR)

    try:
        return open(arg, mode)
    except:
        exit(PARAM_ERR)

# parse input argumnets, open source/input files
def parse_args():
    prg = Program()
    cmds = ['help', 'source=', 'input=', 'insts', 'stats=', 'hot', 'vars']
    tree = None
    try:
        opts, args = getopt.getopt(sys.argv[1:], '', cmds)

        for opt, arg in opts:
            if opt in ('--help'):
                help_msg()
            elif opt in ('--source'):
                tree = read_xml(tree, arg)
            elif opt in ('--input'):
                prg._input_f = open_file(prg, arg, "r", prg._input_f)
            elif opt in ('--insts'):
                prg._print.append('i')
            elif opt in ('--vars'):
                prg._print.append('v')
            elif opt in ('--hot'):
                prg._print.append('h')
            elif opt in ('--stats'):
                prg._file_stats = open_file(prg, arg, "w", prg._file_stats)
            else:
                exit(PARAM_ERR)

    except getopt.GetoptError:
        help_msg()
        exit(PARAM_ERR)

    if len(prg._print) > 0 and prg._file_stats == None:
        exit(PARAM_ERR)

    if prg._input_f == None and tree == None:
        exit(PARAM_ERR)
    elif prg._input_f == None:
        prg._input_f = None
    elif tree == None:
        tree = ET.parse(sys.stdin)

    return prg, tree

# check if arguments of the instruction are correct
def check_args(dict_args, expected_args):
    if (len(dict_args) != expected_args or
        (expected_args == 1 and sum(dict_args.keys()) != 0) or
        (expected_args == 2 and sum(dict_args.keys()) != 1) or
        (expected_args == 3 and sum(dict_args.keys()) != 3)):
        exit(STRUCT_ERR)

def parse_xml(root):
    dict_instr = {}

    for child in root:
        dict_args = dict()
        if (child.tag != "instruction" or "opcode" not in child.attrib or
            "order" not in child.attrib or len(child.attrib) != 2):
            exit(STRUCT_ERR)

        check_order(prg, child.attrib["order"])

        for ch in child:
            if ch.tag[0:3] != "arg" or int(ch.tag[3:len(ch.tag)]) > 3 or int(ch.tag[3:len(ch.tag)]) < 1:
                exit(STRUCT_ERR)

            new_text = ch.text
            # change all sequences like \ddd to UTF symbol
            if ch.text != None and "string" == ch.attrib["type"]:
                new_text = re.sub(r'\\([0-9]{3})', lambda x: chr(int(x[1])), ch.text)

            dict_args[int(ch.tag[3:len(ch.tag)]) - 1] = {ch.attrib["type"] : new_text}

        # add instruction to class (using Abstract Factory)
        instr = Factory.resolve(child.attrib["opcode"].upper(), dict_args)
        dict_instr[int(child.attrib["order"])] = instr
        check_args(dict_args, instr._num_of_arg)

    return dict_instr

prg, tree = parse_args()

root = tree.getroot()
if (not re.match('^IPPcode22$', root.attrib['language']) or
    root.tag != "program"):
    exit(STRUCT_ERR)

# parse xml structure
dict_instr = parse_xml(root)
# sort by order of instruction
dict_instr = sorted(dict_instr.items(), key = lambda i: i[0])
# check if all labels are valid
check_label(prg, dict_instr)

num_instr = len(dict_instr)
while prg._line < num_instr:
    dict_instr[prg._line][1].execute(prg)
    inc_insts(prg, dict_instr[prg._line][1]._opcode, dict_instr[prg._line][0])
    prg._line += 1
