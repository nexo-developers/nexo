import pygame
from pygame.sprite import Sprite

__author__ = 'losangeles'

class Character(Sprite):

    def __init__(self, charid, imagefile, x, y):
        pygame.sprite.Sprite.__init__(self)
        self.id = charid
        self.image = pygame.image.load(imagefile)
        self.rect = self.image.get_rect()
        self.rect.topleft= (int(x), int(y))

    def update(self):
        pass    #aca va la logica de animacion del personaje
