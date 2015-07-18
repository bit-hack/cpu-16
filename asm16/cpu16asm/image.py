class Image(object):

    # 64k of memory space
    image_ = bytearray(0x10000)

    # the emit address
    head_ = 0

    # constructor
    def __init__(self):
        assert self.image_

    # zero out the image
    def clear(self):
        self.image_ = bytearray(0x10000)

    # write byte value to address
    def write_8(self, address, value):
        if type(value) is str:
            assert len(value) == 1
            value = ord(value[0])
        assert 0 <= address <= 0xffff
        assert 0 <= value <= 0xff
        self.image_[address] = value

    # read byte from address
    def read_8(self, address):
        assert 0 <= address <= 0xffff
        return self.image_[address]

    # write word to address
    def write_16(self, address, value):
        assert 0 <= address <= 0xfffe
        if type(value) is str:
            assert len(value) == 1
            lo = value[0]
            hi = value[1]
        else:
            lo = value & 0xff
            hi = value >> 8
        assert 0 <= lo <= 0xff
        assert 0 <= hi <= 0xff
        self.image_[address+0] = lo
        self.image_[address+1] = hi

    # read word from address
    def read_16(self, address):
        assert 0 <= address <= 0xfffe
        lo = self.image_[address+0]
        assert 0 <= lo <= 0xff
        hi = self.image_[address+1]
        assert 0 <= hi <= 0xff
        return (hi << 8) | lo

    # write byte to current head
    def emit_8(self, value):
        assert 0 <= self.head_ <= 0xffff
        self.write_8(self.head_, value)
        self.head_ += 1

    # write word to current head
    def emit_16(self, value):
        assert 0 <= self.head_ <= 0xfffe
        self.write_16(self.head_, value)
        self.head_ += 2

    # emit values from a byte array
    def emit(self, array):
        assert type(array) is bytearray
        for byte in array:
            self.emit_8(byte)

    # set the current emit head
    def set_head(self, address):
        assert 0 <= address <= 0xffff
        self.head_ = address

    # get the current emit head
    def get_head(self):
        return self.head_

    # save image to disk
    def save(self, path):
        with open(path, 'wb') as fd:
            fd.write(self.image_)

    # load image from disk
    def load(self, path):
        with open(path, 'rb') as fd:
            raw = fd.read()
            assert len(raw) == 0x10000
            self.image_ = bytearray(raw)
