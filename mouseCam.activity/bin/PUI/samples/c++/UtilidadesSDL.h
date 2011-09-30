/* @author Sebastián Marichal
 * 		   sebastian_marichal@hotmail.com
 * 		   sebarocker@gmail.com
 *
 * Universidad de la República, Montevideo , Uruguay
 */
#ifndef UTILIDADESSDL_H_INCLUDED
#define UTILIDADESSDL_H_INCLUDED
#include <SDL/SDL_mixer.h>
#include <SDL/SDL.h>
#include <SDL/SDL_gfxPrimitives.h>

#define MAX_IMAGENES 30

int initSDL(int width, int height);
int PlayImage(char* image, char* leyenda, int flip);
int PlayImageConTitulo(char* image, char* titulo,int x, int y, int flip);
int PlayImageSinTitulo(char* image,int x, int y,int flip);
int PlaySound (char* soundfile);
int EscribirTexto(char* texto, int x, int y, int flip);
int CargarFondo(const char * imagen, int flip);
int OcultarImage(int idImagen, int flip);
int OcultarTodo();
int OcultarZona(int x, int y, int width, int height, int flip);
int mostrarImagen(int idImagen, int flip);
int LiberarMemoriaImagen(int idImagen);
void flip();
int cleanUpSDL();
#endif // UTILIDADESSDL_H_INCLUDED
