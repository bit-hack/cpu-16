#!/usr/bin/env python

import sys
import os.path
from cpu16asm import Image, Scope, Instruction, ParseException, ParseExceptionEx

class State(object):
    def __init__(self):
        self.image_ = Image()
        self.scope_ = Scope(self)
        self.line_ = 0

    def next_line(self):
        self.line_ += 1

def read_literal(state, value):
    assert type(value) is str
    assert len(value) > 1
    if value[0] == '#':
        return int(value[1:], 16)
    return None

def read_register(state, value):
    assert type(value) is str
    assert len(value) > 1
    if value == 'ZR':
        return 0
    if value == 'PC':
        return 1
    if value == 'SP':
        return 2
    if value[0] == 'R':
        return int(value[1:])
    return None

def emit_instruction(state, prototypes, operands):
    op_ix = 0
    inst = Instruction()
    # try each prototype in turn
    for proto in prototypes:
        assert len(proto) == 2
        # check there are enough operands
        if len(proto[0]) > len(operands):
            continue
        # clear all instruction fields
        inst.clear()
        op_ix = 0
        # try to match each field in prototype
        fail = False
        for field in proto[0]:
            # parse a register
            if field == 'X' or field == 'Y':
                val = read_register(state, operands[op_ix])
                if val is None:
                    fail = True
                    break
                if field == 'X':
                    inst.set_rx(val)
                elif field == 'Y':
                    inst.set_ry(val)
                op_ix += 1
                continue
            # parse an immediate
            if field == 'I':
                # get the immediate operand
                imm = operands[op_ix]
                # read hex literal
                if imm[0] == '#':
                    if len(imm) <= 1:
                        raise ParseException('malformed literal')
                    val = read_literal(state, operands[op_ix])
                    if val is None:
                        fail = True
                        break
                # read symbolic offset
                elif imm[0] == '%':
                    if len(imm) <= 1:
                        raise ParseException('malformed symbolic offset')
                    val = 0xCCCC
                    state.scope_.add_fixup(
                        state.image_.get_head()+2,
                        imm[1:],
                        state.line_)
                # this is not a kind of immediate
                else:
                    fail = True
                    break
                # we have an immediate
                inst.set_imm(val)
                op_ix += 1
                continue
        # if this encoding is valid
        if not fail:
            inst.set_opcode(proto[1])
            enc = inst.encode()
            state.image_.emit(enc)
            return
    # failed to match
    raise ParseException('invalid instruction operands')

map_LOAD = {
    'LOAD.W':  {('YX', '\x00'), ('IX', '\x10')},
    'LOAD.B':  {('YX', '\x01'), ('IX', '\x11')},
    'LOAD.W+': {('YX', '\x02')},
    'LOAD.B+': {('YX', '\x03')},
    }

map_STORE = {
    'STORE.W':  {('YX', '\x04'), ('XI', '\x14')},
    'STORE.B':  {('YX', '\x05'), ('XI', '\x15')},
    'STORE.W+': {('YX', '\x06')},
    'STORE.B+': {('YX', '\x07')},
    }

map_ALU = {
    'ADD':  {('YX', '\x20'), ('IX', '\x30')},
    'MUL':  {('YX', '\x21'), ('IX', '\x31')},
    'SHL':  {('YX', '\x22'), ('IX', '\x32')},
    'SHR':  {('YX', '\x23'), ('IX', '\x33')},
    'SUB':  {('YX', '\x24'), ('IX', '\x34')},
    'DIV':  {('YX', '\x25'), ('IX', '\x35')},
    'MOD':  {('YX', '\x26'), ('IX', '\x36')},
    'AND':  {('YX', '\x27'), ('IX', '\x37')},
    'OR':   {('YX', '\x28'), ('IX', '\x38')},
    'XOR':  {('YX', '\x29'), ('IX', '\x39')},
    'MOV':  {('YX', '\x2A'), ('IX', '\x3A')},
    'MLH':  {('YX', '\x2B'), ('IX', '\x3B')},
    }

map_JMP_COND = {
    'JNE': {('YXI', '\x51')},
    'JEQ': {('YXI', '\x52')},
    'JL':  {('YXI', '\x53')},
    'JG':  {('YXI', '\x54')},
    'JLE': {('YXI', '\x55')},
    'JGE': {('YXI', '\x56')},
    }

def handle_LOAD(state, operands):
    assert operands[0] in map_LOAD
    emit_instruction(state, map_LOAD[operands[0]], operands[1:])


def handle_STORE(state, operands):
    assert operands[0] in map_STORE
    emit_instruction(state, map_STORE[operands[0]], operands[1:])

def handle_ALU(state, operands):
    assert operands[0] in map_ALU
    emit_instruction(state, map_ALU[operands[0]], operands[1:])

def handle_PUSH(state, operands):
    proto = {('X', '\x40')}
    emit_instruction(state, proto, operands[1:])

def handle_POP(state, operands):
    proto = {('X', '\x41')}
    emit_instruction(state, proto, operands[1:])

def handle_JMP(state, operands):
    proto = {('I', '\x50')}
    emit_instruction(state, proto, operands[1:])

def handle_JMP_COND(state, operands):
    assert operands[0] in map_JMP_COND
    emit_instruction(state, map_JMP_COND[operands[0]], operands[1:])

def handle_CALL(state, operands):
    proto = {('I', '\x70')}
    emit_instruction(state, proto, operands[1:])

def handle_INT(state, operands):
    proto = {('I', '\x71')}
    emit_instruction(state, proto, operands[1:])

def handle_RETI(state, operands):
    proto = {('', '\x80')}
    emit_instruction(state, proto, operands[1:])

def handle_CLI(state, operands):
    proto = {('', '\x81')}
    emit_instruction(state, proto, operands[1:])

def handle_STI(state, operands):
    proto = {('', '\x82')}
    emit_instruction(state, proto, operands[1:])

def handle_NOP(state, operands):
    handle_ALU(state, ['MOV', 'R0', 'R0'])

def handle_NOT(state, operands):
    if len(operands) < 2:
        raise ParseException('expecting operand')
    handle_ALU(state, ['XOR', '#FFFF', operands[1]])
    pass

def handle_SWAP(state, operands):
    rx = operands[1]
    ry = operands[2]
    handle_ALU(state, ['XOR', rx, ry])
    handle_ALU(state, ['XOR', ry, rx])
    handle_ALU(state, ['XOR', rx, ry])

def handle_RET(state, operands):
    handle_ALU(state, ['POP', 'R1'])
    pass

def handle_FUNCTION(state, operands):
    state.scope_.add_symbol(operands[1], state.image_.get_head(), state.line_)
    state.scope_.enter()
    pass

def handle_END(state, operands):
    state.scope_.leave()
    pass

def handle_DATA(state, operands):
    for dat in operands[1:]:
        if len(dat) <= 1:
            raise ParseException('bad data operand')
        if dat[0] == '$':
            state.image_.emit(bytearray(dat[1:]))
            continue
        elif dat[0] == '#':
            val = int(dat[1:], 16)
            if len(dat) == 3:
                state.image_.emit_8(val)
                continue
            if len(dat) == 5:
                state.image_.emit_16(val)
                continue
            raise ParseException('invalid literal')
        elif dat[0] == '%':
            symbol = 0xCCCC
            state.scope_.add_fixup(state.image_.get_head(), dat[1:], state.line_)
            state.image_.emit_16(symbol)
        else:
            raise ParseException('bad data operand')

def handle_LABEL(state, operands):
    label = operands[0]
    assert type(label) is str
    assert len(label) > 1
    label = label[1:]
    state.scope_.add_symbol(label, state.image_.get_head(), state.line_)

def handle_AT(state, operands):
    if len(operands) != 2:
        raise ParseException("expecting 1 argument")
    val = read_literal(state, operands[1])
    if not (0 <= val <= 0xffff):
        raise ParseException("address out of range")
    assert state.image_
    state.image_.set_head(val)

g_map = {
    'LOAD.W':   handle_LOAD,
    'LOAD.B':   handle_LOAD,
    'LOAD.W+':  handle_LOAD,
    'LOAD.B+':  handle_LOAD,
    'STORE.W':  handle_STORE,
    'STORE.B':  handle_STORE,
    'STORE.W+': handle_STORE,
    'STORE.B+': handle_STORE,

    'ADD':      handle_ALU,
    'MUL':      handle_ALU,
    'SHL':      handle_ALU,
    'SHR':      handle_ALU,
    'SUB':      handle_ALU,
    'DIV':      handle_ALU,
    'MOD':      handle_ALU,
    'AND':      handle_ALU,
    'OR':       handle_ALU,
    'XOR':      handle_ALU,
    'MOV':      handle_ALU,
    'MLH':      handle_ALU,

    'PUSH':     handle_PUSH,
    'POP':      handle_POP,

    'JMP':      handle_JMP,
    'JNE':      handle_JMP_COND,
    'JEQ':      handle_JMP_COND,
    'JL':       handle_JMP_COND,
    'JG':       handle_JMP_COND,
    'JLE':      handle_JMP_COND,
    'JGE':      handle_JMP_COND,

    'CALL':     handle_CALL,
    'RETI':     handle_RETI,

    'CLI':      handle_CLI,
    'STI':      handle_STI,

    'NOP':      handle_NOP,
    'NOT':      handle_NOT,
    'SWAP':     handle_SWAP,
    'RET':      handle_RET,

    'FUNCTION': handle_FUNCTION,
    'END':      handle_END,
    'DATA':     handle_DATA,
    'AT':       handle_AT,
    }

def parse_line(state, tokens):
    assert len(tokens) > 0
    mnemonic = tokens[0]
    assert len(mnemonic) >= 1
    # check for comment
    if mnemonic[0] == '-':
        return
    # check for label
    if mnemonic[0] == '.':
        handle_LABEL(state, tokens)
        return
    # check for mnemonic/directive
    if mnemonic not in g_map:
        raise ParseException('unknown directive {0}'.format(mnemonic))
    handler = g_map[mnemonic]
    assert handler
    handler(state, tokens)

def parse_source(state, source):
    for line in source:
        state.next_line()
        line = line.upper()
        tokens = line.split()
        if len(tokens) == 0:
            pass  # blank line
        else:
            parse_line(state, tokens)

def load_source(path):
    with open(path, "r") as fd:
        return fd.readlines()

def usage():
    file = os.path.basename(__file__)
    print 'cpu16 assembler - v1.0'
    print '{0} <input.asm> <output.img>'.format(file)

def main():

    # check arguments are provided
    args = sys.argv
    if len(args) != 3:
        usage()
        quit()

    state = State()
    try:
        source = load_source(args[1])
        if not source:
            raise ParseException('unable to load input file')
        state.scope_.enter()
        parse_source(state, source)
        state.scope_.leave()
        state.image_.save(args[2])
    except ParseException as error:
        print 'line {0}: {1}'.format(state.line_, str(error))
    except ParseExceptionEx as error:
        print 'line {0}: {1}'.format(error.line_, str(error))
    quit()

main()
