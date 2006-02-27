
def Read(s):
    startPos = s.find('(')
    endPos = s.rfind(')')

    if startPos != -1 and endPos != -1:
        return [
            s[:startPos].split() +
            Read(s[startPos + 1 : endPos]) +
            s[endPos + 1:].split()]
            
    else:
        return [s.split()]