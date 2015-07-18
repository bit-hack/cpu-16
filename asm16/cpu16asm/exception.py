class ParseException(Exception):
    pass

class ParseExceptionEx(Exception):
    def __init__(self, msg, line):
        self.line_ = line
        self.msg_ = msg

    def __str__(self):
        return self.msg_
