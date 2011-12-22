import xml.dom.minidom
__author__ = 'losangeles'

class Test(object):

    def __init__(self):
        pass

    def parse(self):
        some = xml.dom.minidom.parse("../../../resources/Scenes.xml")
        charnodes = some.getElementsByTagName("characters")[0]

        childs = charnodes.getElementsByTagName("character")
        print len(childs)
        for child in childs:
            charid = child.getAttribute("id")
            charsprite = child.getElementsByTagName("sprite")[0].getAttribute("value")

if __name__ == '__main__':
    test = Test()
    test.parse()
