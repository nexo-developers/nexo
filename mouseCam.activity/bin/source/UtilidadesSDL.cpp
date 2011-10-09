/* @author Sebastián Marichal
 * 		   sebastian_marichal@hotmail.com
 * 		   sebarocker@gmail.com
 *
 * Universidad de la República, Montevideo , Uruguay
 */
#include "UtilidadesSDL.h"

Mix_Chunk *sound = NULL;		//Pointer to our sound, in memory
int channel;				//Channel on which our sound is played
SDL_Surface *screen;
int audio_rate = 22050;			//Frequency of audio playback
Uint16 audio_format = AUDIO_S16SYS; 	//Format of the audio we're playing
int audio_channels = 2;			//2 channels = stereo
int audio_buffers = 4096;		//Size of the audio buffers in memory

SDL_Rect rectangulos[MAX_IMAGENES];
SDL_Surface* imagenes[MAX_IMAGENES];
int tope = -1;

SDL_Rect fondo_rect;
SDL_Surface* fondo;

int width = 0;
int height = 0;

int initSDL(int width, int height){
    //Initialize BOTH SDL video and SDL audio
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
		printf("Unable to initialize SDL: %s\n", SDL_GetError());
		return 0;
	}else {
		/*Titulo de la ventana*/
	    SDL_WM_SetCaption( "Prototipo", "Prototipo");
		screen = SDL_SetVideoMode( width, height, 16, SDL_HWSURFACE );
		if( screen == NULL ) {
		  printf( "Error al entrar a modo grafico: %s\n", SDL_GetError() );
		  SDL_Quit();
		  return 0;
	    }

		/*Inicializo Sonido*/
		//Initialize SDL_mixer with our chosen audio settings
		if(Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffers) != 0) {
			printf("Unable to initialize audio: %s\n", Mix_GetError());
			exit(1);
		}
		return 1;
	}
}

int CargarFondo(const char * imagen, int flip){
	fondo = NULL;
	fondo = SDL_LoadBMP(imagen);
	if(fondo!=NULL){
		fondo_rect.x = 0;
		fondo_rect.y = 0;

		SDL_BlitSurface(fondo, NULL, screen, &fondo_rect);
		if(flip){
			SDL_Flip(screen);
		}
	}else{
		fprintf(stderr, "Error cargando fondo\n");
		return -1;
	}
	return 1;

}

int PlayImage(char* image, char* leyenda,int flip){
    SDL_Surface *imagen = NULL;
    imagen = SDL_LoadBMP(image);
   /*Titulo de la ventana*/
   SDL_WM_SetCaption( leyenda, leyenda);
   SDL_Rect destino;
    /* Dibujamos la imagen de fondo */
    destino.x=5;
    destino.y=5;
    SDL_BlitSurface(imagen, NULL, screen, &destino);

    if(flip){
    	/*Actualizo la pantalla*/
    	SDL_Flip(screen);
    }

}


int EscribirTexto(char* texto, int x, int y,int flip){
	//TODO conseguir fuentes
	//gfxPrimitivesSetFont(&SDL_gfx_font_7x13B_fnt,7,13);
	stringRGBA(screen,x,y,texto,255,255,255,255);
	if(flip){
		SDL_Flip(screen);
	}
}




/**
 * Carga la imagen en las coordenadas (x,y) de la pantalla.
 * Si se carga exitosamente retorna el id de la imagen, de lo contrario retorna -1
 */
int PlayImageConTitulo(char* image, char* titulo,int x, int y,int flip){
	printf("Imagen = %s\n",image);
	tope++;
	imagenes[tope] = NULL;
	imagenes[tope] = SDL_LoadBMP(image);

    if(imagenes[tope]!=NULL){
		/* Dibujamos la imagen de fondo */
		rectangulos[tope].x=x;
		rectangulos[tope].y=y;
		SDL_BlitSurface(imagenes[tope], NULL, screen, &rectangulos[tope]);

	//	OcultarZona(x,y-30,imagenes[tope]->w,imagenes[tope]->h,0);
		EscribirTexto(titulo,x,y-30,0);
		if(flip){
			/*Actualizo la pantalla*/
			SDL_Flip(screen);
		}
		return tope;
    }else{
    	tope--;
    	fprintf(stderr, "Error cargando imagen\n");
    	return -1;
    }
}

/**
 * Carga la imagen en las coordenadas (x,y) de la pantalla.
 * Si se carga exitosamente retorna el id de la imagen, de lo contrario retorna -1
 */
int PlayImageSinTitulo(char* image,int x, int y,int flip){
	printf("Imagen = %s\n",image);
	tope++;
	imagenes[tope] = NULL;
	imagenes[tope] = SDL_LoadBMP(image);

    if(imagenes[tope]!=NULL){
		/* Dibujamos la imagen de fondo */
		rectangulos[tope].x=x;
		rectangulos[tope].y=y;
		SDL_BlitSurface(imagenes[tope], NULL, screen, &rectangulos[tope]);

		if(flip){
			/*Actualizo la pantalla*/
			SDL_Flip(screen);
		}
		return tope;
    }else{
    	tope--;
    	fprintf(stderr, "Error cargando imagen\n");
    	return -1;
    }
}




int OcultarImage(int idImagen, int flip){
	if(idImagen<0 || idImagen>MAX_IMAGENES){
		fprintf(stderr, "Error ocultando imagen, el id de la imagen es invalido\n");
		return -1;
	}else{
		if(imagenes[idImagen]!=NULL){
			/* Dibujamos la imagen de fondo */
			//Parametros de BlitSurface SDL_Surface *src, SDL_Rect *srcrect, SDL_Surface *dst, SDL_Rect *dstrect
			SDL_BlitSurface(fondo, &rectangulos[idImagen], screen, &rectangulos[idImagen]);

			if(flip){
				/*Actualizo la pantalla*/
				SDL_Flip(screen);
			}
			return 1;
		}else{
			fprintf(stderr, "Error ocultando imagen\n");
			return -1;
		}
	}
}

int mostrarImagen(int idImagen, int flip){
	if(idImagen<0 || idImagen>MAX_IMAGENES){
		fprintf(stderr, "Error mostrando imagen, el id de la imagen es invalido\n");
		return -1;
	}else{
		if(imagenes[idImagen]!=NULL){
			/* Dibujamos la imagen de fondo */
			//Parametros de BlitSurface SDL_Surface *src, SDL_Rect *srcrect, SDL_Surface *dst, SDL_Rect *dstrect
			SDL_BlitSurface(imagenes[idImagen], NULL, screen, &rectangulos[idImagen]);
			/*Actualizo la pantalla*/
			if(flip){
				SDL_Flip(screen);
			}
			return 1;
		}else{
			fprintf(stderr, "Error mostrando imagen\n");
			return -1;
		}
	}
}

void flip(){
	SDL_Flip(screen);
}

int OcultarTodo(){
	if( fondo != NULL){
		SDL_BlitSurface(fondo, &fondo_rect, screen, &fondo_rect);
		/*Actualizo la pantalla*/
		SDL_Flip(screen);
	}
}


int OcultarZona(int x, int y, int width, int height,int flip){
	if( fondo != NULL){
		SDL_Rect rect;
		rect.x = x;
		rect.y = y;
		rect.w = width;
		rect.h = height;
		SDL_BlitSurface(fondo, &rect, screen, &rect);
		if(flip){
			/*Actualizo la pantalla*/
			SDL_Flip(screen);
		}
		return 1;
	}else{
		return -1;
	}
}


int LiberarMemoriaImagen(int idImagen){
	//TODO ver cuando hay que invocarla
	if(idImagen<0){
		fprintf(stderr, "Error, idImagen incorrecto\n");
		return -1;
	}else{
		if(imagenes[idImagen]!=NULL){
			SDL_FreeSurface(imagenes[idImagen]);
			/*Actualizo la pantalla*/
			SDL_Flip(screen);
		}else{
			fprintf(stderr, "Error, la imagen que quiere quitarse es NULL\n");
			return -1;
		}

	}

	SDL_Flip(screen);
	return 1;
}

int ActualizarImagen(char* image, char* titulo){

}

/*int limpiarPantalla(){

}*/


int cleanUpSDL(){
	//Need to make sure that SDL_mixer and SDL have a chance to clean up
	Mix_CloseAudio();
	SDL_Quit();
}

int PlaySound (char* soundfile){

	//Load our WAV file from disk
	sound = Mix_LoadWAV(soundfile);
	if(sound == NULL) {
		printf("Unable to load WAV file: %s\n", Mix_GetError());
	}

	//Play our sound file, and capture the channel on which it is played
	channel = Mix_PlayChannel(-1, sound, 0);
	if(channel == -1) {
		printf("Unable to play WAV file: %s\n", Mix_GetError());
	}

	//Wait until the sound has stopped playing
	while(Mix_Playing(channel) != 0);

	//Release the memory allocated to our sound
	Mix_FreeChunk(sound);

	//Return success!
	return 0;
}
