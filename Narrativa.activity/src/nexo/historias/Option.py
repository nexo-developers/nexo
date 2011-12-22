import pygame
from pygame.sprite import Sprite

__author__ = 'losangeles'

class Option(Sprite):
    def __init__(self, id, image, posx, posy, action, persId):
        pygame.sprite.Sprite.__init__(self)
        
        self.id = id
        self.image = pygame.image.load(image)
        self.rect = self.image.get_rect()
        self.rect.topleft = (int(posx), int(posy))
        self.action = action
        self.personajeId = persId

    def update(self, *args):
        pass
