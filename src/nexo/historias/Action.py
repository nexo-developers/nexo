__author__ = 'losangeles'

#Los parametros x,y solo tienen sentido cuando verb es WALK o JUMP, speed solo tiene sentido en WALK.
#Los parametros que no tienen sentido se ignoraran en caso de ser pasados de todas formas
class Action(object):
    WALK = 0
    JUMP = 1
    PLAYSOUND = 2

    def __init__(self, verb, resource, x, y, speed):
        self.verb = verb
        self.resource = resource
        self.x = x
        self.y = y
        self.speed = speed
