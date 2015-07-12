class Instruction(object):

    opcode_ = 0
    rx_ = 0
    ry_ = 0
    imm_ = None

    def __init__(self):
        self.clear()

    def clear(self):
        self.opcode_ = 0
        self.rx_ = 0
        self.ry_ = 0
        self.imm_ = None

    def set_rx(self, reg):
        assert 0 <= reg <= 15
        self.rx_ = reg

    def set_ry(self, reg):
        assert 0 <= reg <= 15
        self.ry_ = reg

    def set_imm(self, imm):
        assert 0 <= imm <= 0xffff
        self.imm_ = imm

    def set_opcode(self, opcode):
        if type(opcode) is str:
            opcode = ord(opcode)
        assert 0 <= opcode <= 0xff
        self.opcode_ = opcode

    def encode(self):
        out = bytearray(2)
        out[0] = self.opcode_
        out[1] = (self.ry_ << 4) | self.rx_
        if self.imm_:
            out.append(self.imm_ & 0xff)
            out.append(self.imm_ >> 8)
        return out
