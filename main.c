#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define QUITKEY SDLK_ESCAPE
#define WIDTH 		480
#define HEIGHT 		640
#define BLOCKSIZE 	30
#define FIELDH 		20
#define FIELDW 		10
#define MLEFT 		1
#define MRIGHT 		2
#define RLEFT 		4
#define RRIGHT 		8
#define FALL 		16

#define TRUE		1
#define FALSE		0

#define MOVEABLE 	1
#define RESIDENT	65

//global vars
int quit=0;

//SDL Declaration
int errorCount=0 ;
SDL_Window* screen = NULL;
SDL_Renderer* renderer;
SDL_Event event;
SDL_Rect rect, source, destination, dst;

//Tetris Declarations

unsigned char spielfeld[ FIELDW*FIELDH ];

/*
 *	 0	 1	 2	 3
 *	10	11	12	13
 * 	20	21	22	23
 * 	30	31	32	33
 *
 * minimale x pos und maximale x pos
 */

int tm [7][4][6]= {
	// T
	{ {  1,10,11,12, 0,7 } , {  1,11,12,21,-1,7 } , { 10,11,12,21, 0,7 } , {  1,11,10,21, 0,8 } } ,
	// L gespiegelt
	{ {  1, 2,11,21,-1,7 } , { 10,11,12,22, 0,7 } , {  1,11,20,21, 0,8 } , {  0,10,11,12, 0,7 } } ,
	// L
	{ {  1,11,21,22,-1,7 } , { 10,11,12,20, 0,7 } , {  0, 1,11,21, 0,8 } , {  2,10,11,12, 0,7 } } ,
	// I
	{ { 10,11,12,13, 0,6 } , {  2,12,22,32,-2,7 } , { 20,21,22,23, 0,6 } , {  1,11,21,31,-1,8 } } ,
	// Z
	{ {  0, 1,11,12, 0,7 } , {  2,11,12,21,-1,7 } , { 10,11,21,22, 0,7 } , {  1,10,11,20, 0,8 } } ,
	// S
	{ {  1, 2,10,11, 0,7 } , {  1,11,12,22,-1,7 } , { 11,12,20,21, 0,7 } , {  0,10,11,21, 0,8 } } ,
	// o
	{ {  0, 1,10,11, 0,8 } , {  0, 1,10,11, 0,8 } , {  0, 1,10,11, 0,8 } , {  0, 1,10,11, 0,8 } }
} ;


//SDL Stuff
void LogError(char* msg) {
	errorCount++;
	printf( msg," %d",errorCount );
	printf("SDL Error: %s\n", SDL_GetError() );

}

void InitSetup() {
	srand((int)time(NULL));
	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, SDL_WINDOW_SHOWN, &screen, &renderer);
	if (!screen) {
		LogError("InitSetup failed to create window");
	}
	SDL_SetWindowTitle( screen,"Tetris" );
}

void ExitClean() {
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(screen);
	//Quit SDL
	SDL_Quit();
}

int main( void ){
	int quit=FALSE;
	int ticks=0 ;
	int fixed=FALSE;
	int key;
	int mino, minonext,tetriscount;

	srand(time(NULL));

	while(!quit){

		int gameover=0 ;
		int xmino, ymino, rotation ;
		int check;

		// SDL
		InitSetup();

		// Titel

		minonext = rand() % 7 ;

		clearplayfield(RESIDENT);

		while (!gameover){

			// Spiel
			// zeichne Spielfeld Dekor

			mino = minonext ;
			minonext = rand() % 7;

			ymino=0 ; xmino=3 ; rotation=0 ;
			fixed=0;

			if ( !checkmino (xmino,ymino,mino,rotation) ) gameover=1;

			while (!fixed) {

				key=pressed();

				ticks++;
				if ( (ticks % 5000)==0 ) { 
					key=FALL;
				}

				switch (key) {
					case MLEFT:
					    xmino-- ;
					    //? Rand
					    if ( xmino < tm[mino][rotation][4] ) xmino=tm[mino][rotation][4];

					    //? Spielfeld
					    if (checkmino(xmino,ymino,mino,rotation)) {
							putmino(xmino,ymino,mino,rotation,MOVEABLE);
						}
					    else {
							xmino++;
							putmino(xmino,ymino,mino,rotation,MOVEABLE);
						}
					break ;

					case MRIGHT:
						xmino++;
						// ? Rand
						if ( xmino > tm[mino][rotation][5] ) xmino=tm[mino][rotation][5];

						//? Spielfeld
						if (checkmino(xmino,ymino,mino,rotation)) {
							putmino(xmino,ymino,mino,rotation,MOVEABLE);
						}
						else {
							xmino--;
							putmino(xmino,ymino,mino,rotation,MOVEABLE);
						}
					break ;

					case RLEFT:
						rotation-- ; rotation&=3 ;
						if (checkmino(xmino,ymino,mino,rotation)) {
							putmino(xmino,ymino,mino,rotation,MOVEABLE);
						}
					    	else {
							rotation++; rotation&3 ;
							putmino(xmino,ymino,mino,rotation,MOVEABLE);
						}
					break;

					case RRIGHT:
						rotation++ ; rotation&=3 ;
						if (checkmino(xmino,ymino,mino,rotation)) {
							putmino(xmino,ymino,mino,rotation,MOVEABLE);
						}
					    	else {
							rotation--; rotation&3 ;
							putmino(xmino,ymino,mino,rotation,MOVEABLE);
						}
					break;

					case FALL:
						ymino++;
						if ( checkmino(xmino,ymino,mino,rotation)==TRUE ) {
							putmino (xmino,ymino,mino,rotation,MOVEABLE);
						}
						else {
							fixed=TRUE;
							putmino (xmino,ymino-1,mino,rotation,RESIDENT);
							ymino=0 ; xmino=3 ;
						}
					break;

					default:
						putmino (xmino,ymino,mino,rotation,MOVEABLE);
					break;
				}

				DrawPlayfield();
				clearplayfield(MOVEABLE);
			}

			tetriscount=0 ;
			for (check=FIELDH-1 ; check>0 ; check--) {
				while ( checkline(check) ) {
					clearline(check);
					tetriscount++ ;
				}
			}
		}
	}

	ExitClean();
	return (0);
}

int checkmino ( int x,int y, int m, int r) {

	int i;
	int frei=TRUE;
	int unten=FALSE;
	int adr;
	
	// ? Rand
	if ( x < tm[m][r][4] ) x=tm[m][r][4];
	if ( x > tm[m][r][5] ) x=tm[m][r][5];
	
	//prüfe ob neue position frei ist
	for ( i=0 ; i<4; i++ ) {
		adr=x+y*FIELDW+tm[m][r][i];
		if (adr<(FIELDW*FIELDH)) {
			frei&=( spielfeld[ adr ] == 0 ) ;
		}
		else {
			unten=TRUE ;
		}
	}
	
	if (unten)
		return(FALSE);
	else
		return(frei);
}

int checkline(int y){
	int i,x,full;
	
	full=TRUE;
	
	i=y*FIELDW ;
	
	for (x=0 ; x<FIELDW ; x++) {
		full&=( spielfeld[i+x]>64);
	}
	return (full) ;
}

void clearline(int y){
	
	int yy,x ;
	
	for (yy=y ; yy>0 ; yy--) {
		for (x=0 ; x<FIELDW ; x++) {
			spielfeld[yy*FIELDW+x]=spielfeld[(yy-1)*FIELDW+x];
			DrawPlayfield();
		}
	}
}

void clearplayfield( int byte ) {
	int i;
	
	if (byte==RESIDENT) {
		for ( i=0 ; i<(FIELDW*FIELDH) ; i++ ) spielfeld[i]=0 ;
	}
	
	if (byte==MOVEABLE) {
		for ( i=0 ; i<(FIELDW*FIELDH) ; i++ ) {
			if ( spielfeld[i]<(unsigned char)65 ) {
				spielfeld[i]=(unsigned char)0;
			}
		}
	}
}

void putmino( int x, int y, int m, int r, int res ){
	
	int i;
	
	clearplayfield(MOVEABLE);
	
	for ( i=0 ; i<4; i++ ) {
			spielfeld[ x+y*FIELDW+tm[m][r][i] ] = (unsigned char)(m+res) ;
	}
}

void DrawPlayfield( void ) {

	int i,x,y,r,g,b ;
	unsigned char s;
	
	for (i=0 ; i<(FIELDW*FIELDH) ; i++ ) {
		
		s=spielfeld[i] ;
		if (s>64) s=s-64 ;
		
		switch (s) {
			case 1  : r=255 ; g=0   ; b=0   ; break ;
			case 2  : r=0   ; g=255 ; b=0   ; break ;
			case 3  : r=0   ; g=0   ; b=255 ; break ;
			case 4  : r=255 ; g=255 ; b=0   ; break ;
			case 5  : r=0   ; g=255 ; b=255 ; break ;
			case 6  : r=255 ; g=0   ; b=255 ; break ;
			case 7  : r=128 ; g=128 ; b=128 ; break ;
			default: r=30 ; g=30  ; b=30  ; break ;
		}
		
		rect.y=(((i) / FIELDW) * BLOCKSIZE ) + 15 ; 
		rect.x=(((i) % FIELDW) * BLOCKSIZE ) + 15 ; 
		rect.w= BLOCKSIZE-1 ;
		rect.h= BLOCKSIZE-1 ;
		SDL_SetRenderDrawColor(renderer,r,g,b,255);
		SDL_RenderFillRect(renderer, &rect);
	}
	SDL_RenderPresent(renderer);
}

int pressed ( void ) {
	int k=0 ;
	
	while (SDL_PollEvent(&event))
	{
		switch (event.type) {
			case SDL_KEYDOWN:
				switch(event.key.keysym.sym){
					case SDLK_LEFT:
						k=1 ;
					break;
					case SDLK_RIGHT:
						k=2 ;
					break;
					case SDLK_a:
						k=4 ;
					break;
					case SDLK_s:
						k=8 ;
					break;
					case SDLK_DOWN:
						k=16 ;
					break;
					default:
						k=0 ;
					break;
				}
			break;
				
			case SDL_KEYUP:  
				switch(event.key.keysym.sym){
					case SDLK_LEFT:
						k=0 ;
					break;
					case SDLK_RIGHT:
						k=0 ;
					break;
					case SDLK_a:
						k=0 ;
					break;
					case SDLK_s:
						k=0 ;
					break;
					case SDLK_DOWN:
						k=0 ;
					default:
					break;
				}
			break;
				
			case SDL_QUIT:
				quit = 1;
			break;
				
			default:
			break;	
		}
	}
	return (k) ;
}
