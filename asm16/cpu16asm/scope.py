from exception import ParseException, ParseExceptionEx

class Scope(object):

    class Pair(object):
        def __init__(self, name, address, line):
            self.name_ = name
            self.address_ = address
            # extra info
            self.line_ = line

    class Block(object):
        def __init__(self):
            self.symbols_ = dict()
            self.fixup_ = list()

    scope_ = list()
    state_ = None

    def __init__(self, state):
        self.state_ = state

    def enter(self):
        self.scope_.append(self.Block())

    def apply_fixup(self, fixup, symbol):
        assert self.state_
        self.state_.image_.write_16(fixup.address_, symbol.address_)

    def leave(self):
        top1 = self.top()
        top2 = None
        if len(self.scope_) >= 2:
            top2 = self.scope_[-2]
        for fixup in top1.fixup_:
            symbol = self.find_symbol(fixup.name_)
            if symbol:
                self.apply_fixup(fixup, symbol)
            else:
                if top2:
                    top2.fixup_.append(fixup)
                else:
                    raise ParseExceptionEx(
                        'unknown symbol {0}'.format(fixup.name_),
                        fixup.line_)
        self.scope_.pop()

    def add_symbol(self, name, address, line):
        assert type(name) is str
        assert type(address) is int
        top = self.top()
        if name in top.symbols_:
            raise ParseExceptionEx(
                'duplicate symbol {0}'.format(name),
                line)
        top.symbols_[name] = self.Pair(name, address, line)

    def add_fixup(self, address, name, line):
        assert type(address) is int
        assert type(name) is str
        top = self.top()
        top.fixup_.append(self.Pair(name, address, line))

    def top(self):
        assert len(self.scope_) > 0
        top = self.scope_[-1]
        assert top
        return top

    def find_symbol(self, name):
        for scope in self.scope_:
            if name in scope.symbols_:
                return scope.symbols_[name]
        return None
