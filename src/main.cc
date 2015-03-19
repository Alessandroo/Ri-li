//      (_||_/
//      (    )       Programme Principale
//     ( o  0 )
//-OOO°--(_)---°OOO---------------------------------------
//                   Copyright (C) 2006 By Dominique Roux-Serret
// .OOOo      oOOO.  roux-serret@ifrance.com
//-(   )------(   )---------------------------------------
//  ( (        ) /   Le 03/01/2006
//   (_)      (_/

//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 or version 3 of the License.

//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.

//    You should have received a copy of the GNU General Public License along
//    with this program; if not, write to the Free Software Foundation, Inc.,
//    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

#include <stdio.h>
#include <stdlib.h>
#include <iostream.h>
#include <string.h>
#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>

#include "preference.h"
#include "jeux.h"
#include "audio.h"
#include "sprite.h"
#include "ecran.h"
#include "mouse.h"
#include "menu.h"
#include "tableau.h"
#include "editeur.h"
#include "utils.h"

/*** Variables globales ***/
/************************/
SDL_Surface *sdlVideo; // Pointe sur l'écran video
SDL_Surface *sdlRVideo; // ZX: vraie video
SDL_VideoInfo *sdlVideoInfo; // Infos sur la video
Uint32 FontColor;      // Couleur du fond d'écran

char Titre[]="Ri-li V2.0.1";

Sprite *Sprites=NULL; // Pointe sur les sprites
int NSprites=0; // Nombre de sprites en mémoire
Ecran Ec[2];          // Pointe sur les 2 buffets vidéo
sPreference Pref;     // Tableau des préférences.
Jeux Jeu;             // Gère le jeu
Mouse Sourie;         // Gère les mouvements de sourie
Menu MenuPrincipale;  // Gère les menus
Tableau Niveau;       // Gère les niveaux
Editeur Edit;         // Gère le menu de l'éditeur
Audio Sons;           // Gère les sons

int Horloge=0; // Horloges du jeu
int HorlogeAvant=0;

#ifdef LINUX
char DefPath[256]; // Chemin par defaut dans arg
#endif

/*** Initialise les preferences ***/
/**********************************/
void InitPref(void)
{
#ifdef LINUX
  DefPath[0]=0;
#endif

  Pref.NiveauMax=0;
# ifdef PND_MODE
  Pref.FullScreen=true;
# else
  Pref.FullScreen=false;
# endif
  Pref.Langue=-1;
  Pref.Volume=(float)SDL_MIX_MAXVOLUME;
  Pref.VolumeM=(float)SDL_MIX_MAXVOLUME; //*6.0/10.0;

  for(int i=0;i<8;i++) { // Vide les scores
    Pref.Sco[i].Score=0;
    Pref.Sco[i].Name[0]=0;
  }
/*   Pref.Sco[0].Score=11425; */
/*   sprintf(Pref.Sco[0].Name,"%s","Dominique"); */
/*   Pref.Sco[1].Score=678; */
/*   sprintf(Pref.Sco[1].Name,"%s","Veronique"); */

  LoadPref();

  Pref.Difficulte=Normal;
  Pref.Vitesse=VITESSE_MOY;
  Pref.VitesseMoy=VITESSE_MOY;
  Pref.NVie=N_VIES_DEP;
  Pref.EcartWagon=ECARTWAGON_MOY;
}

#ifdef CAANOO_MODE
enum
{
     BTN_A = 0,     //       A /             1
     BTN_X = 1,     //       X /             2
     BTN_B = 2,     //       B /             3
     BTN_Y = 3,     //       Y /             4
     BTN_L = 4,     //       L /         5, L1
     BTN_R = 5,     //       R /         6, L2
     BTN_HOME = 6,  //    Home /         7, R1
     BTN_HOLD = 7,  //    Hold /         8, R2
     BTN_HELP1 = 8, //  Help I /        Select
     BTN_HELP2 = 9, // Help II /         Start
     BTN_TACT = 10, //    Tact / L Thumb Stick
     BTN_UP = 11, 
     BTN_DOWN = 12,
     BTN_LEFT = 13,
     BTN_RIGHT = 14,
     // DIAGONALS
     BTN_UP_LEFT = 15,
     BTN_UP_RIGHT = 16,
     BTN_DOWN_LEFT = 17,
     BTN_DOWN_RIGHT = 18
};

//caanoo buttons codes

#define GP2X_A               (0)
#define GP2X_X               (1)
#define GP2X_B               (2)
#define GP2X_Y               (3)
#define GP2X_L               (4)
#define GP2X_R               (5)
#define GP2X_HOME            (6)        
#define GP2X_HOLD            (7)
#define GP2X_SELECT          (8)
#define GP2X_START           (9)        
#define GP2X_FIRE            (10)

#define GP2X_UP              (11)
#define GP2X_DOWN            (12)
#define GP2X_LEFT            (13)
#define GP2X_RIGHT           (14)
#define GP2X_UPLEFT          (15)
#define GP2X_UPRIGHT         (16)
#define GP2X_DOWNLEFT        (17)
#define GP2X_DOWNRIGHT       (18)

void Caanoo_PushAnalogEvent(SDLKey sym, int pressed)
{
        
    SDL_Event event;
        
    event.type = (pressed)?SDL_KEYDOWN:SDL_KEYUP;
    event.key.keysym.sym = sym;
    event.key.state = (pressed)?SDL_PRESSED:SDL_RELEASED;
    event.key.which = 0;
        
    SDL_PushEvent(&event);      
}

void Caanoo_RemapKeyEvent(SDL_Event* event )
{
  if ((event->type != SDL_JOYBUTTONDOWN) &&
      (event->type != SDL_JOYBUTTONUP  )) return;

  int pressed = (event->type == SDL_JOYBUTTONDOWN);

  int  btn_evt = event->jbutton.button;
 
  switch (btn_evt) {
      case GP2X_UP        : Caanoo_PushAnalogEvent(SDLK_UP, pressed);
      break;
      case GP2X_DOWN      : Caanoo_PushAnalogEvent(SDLK_DOWN, pressed);
      break;
      case GP2X_LEFT      : Caanoo_PushAnalogEvent(SDLK_LEFT, pressed);
      break;
      case GP2X_RIGHT     : Caanoo_PushAnalogEvent(SDLK_RIGHT, pressed);
      break;
      case GP2X_UPLEFT    : Caanoo_PushAnalogEvent(SDLK_UP  , pressed);
                            Caanoo_PushAnalogEvent(SDLK_LEFT, pressed);
      break;
      case GP2X_UPRIGHT   : Caanoo_PushAnalogEvent(SDLK_UP   , pressed);
                            Caanoo_PushAnalogEvent(SDLK_RIGHT, pressed);
      break;
      case GP2X_DOWNLEFT  : Caanoo_PushAnalogEvent(SDLK_DOWN, pressed);
                            Caanoo_PushAnalogEvent(SDLK_LEFT, pressed);
      break;
      case GP2X_DOWNRIGHT : Caanoo_PushAnalogEvent(SDLK_DOWN, pressed);
                            Caanoo_PushAnalogEvent(SDLK_RIGHT, pressed);
      break;
      case GP2X_A         : Caanoo_PushAnalogEvent(SDLK_ESCAPE, pressed);
      break;
      case GP2X_B         : Caanoo_PushAnalogEvent(SDLK_RETURN, pressed);
      break;
      case GP2X_X         : Caanoo_PushAnalogEvent(SDLK_DELETE, pressed);
      break;
      case GP2X_Y         : Caanoo_PushAnalogEvent(SDLK_INSERT, pressed);
      break;
      case GP2X_L         : Caanoo_PushAnalogEvent(SDLK_PAGEUP, pressed);
      break;
      case GP2X_R         : Caanoo_PushAnalogEvent(SDLK_PAGEDOWN, pressed);
      break;
      case GP2X_START     : Caanoo_PushAnalogEvent(SDLK_ESCAPE, pressed);
      break;
      case GP2X_SELECT    : Caanoo_PushAnalogEvent(SDLK_ESCAPE, pressed);
      break;
    }
}

SDL_Joystick* psp_sdl_joy;

void Caanoo_UpdateAnalog(void)
{
    static int buttonsPrev = 0;
    int buttonsNow = 0;
        
    SDL_JoystickUpdate();

    int joy_x = SDL_JoystickGetAxis( psp_sdl_joy, 0 );
    int joy_y = SDL_JoystickGetAxis( psp_sdl_joy, 1 );
                
    if (joy_x < -11585) buttonsNow |= (1 << BTN_LEFT);
    if (joy_x >  11585) buttonsNow |= (1 << BTN_RIGHT);
    if (joy_y < -11585) buttonsNow |= (1 << BTN_UP);
    if (joy_y >  11585) buttonsNow |= (1 << BTN_DOWN);
    
    if (buttonsNow != buttonsPrev)
    {
        if ((buttonsNow & (1 << BTN_LEFT)) != (buttonsPrev & (1 << BTN_LEFT)))
        {
                Caanoo_PushAnalogEvent(SDLK_LEFT, (buttonsNow & (1 << BTN_LEFT)));
        }
        if ((buttonsNow & (1 << BTN_RIGHT)) != (buttonsPrev & (1 << BTN_RIGHT)))
        {
                Caanoo_PushAnalogEvent(SDLK_RIGHT, (buttonsNow & (1 << BTN_RIGHT)));
        }
        if ((buttonsNow & (1 << BTN_UP)) != (buttonsPrev & (1 << BTN_UP)))
        {
                Caanoo_PushAnalogEvent(SDLK_UP, (buttonsNow & (1 << BTN_UP)));
        }
        if ((buttonsNow & (1 << BTN_DOWN)) != (buttonsPrev & (1 << BTN_DOWN)))
        {
                Caanoo_PushAnalogEvent(SDLK_DOWN, (buttonsNow & (1 << BTN_DOWN)));
        }
    }
    buttonsPrev = buttonsNow;
}
#endif

// ZX:
static inline void 
loc_do_fast_blit()
{
# if 0
  typedef unsigned int uint32;
  uint32 *src = (uint32 *)sdlVideo->pixels;
  uint32 *tgt = (uint32 *)sdlRVideo->pixels;
  int h = 240;
  while (h-- > 0) {
    int w = 400;
    while (w-- > 0) {
      *tgt++ = *src++;
    }
    if ((h & 0x3) == 0) src += 400;
  }
# else
  SDL_SoftStretch( sdlVideo, NULL, sdlRVideo, NULL );
# endif
}

void
my_SDL_Flip( SDL_Surface* surf )
{
  if (surf == sdlVideo) { 
    // Blit sdlVideo to sdlRVideo
    loc_do_fast_blit();
    SDL_Flip( sdlRVideo );
  } else {
    SDL_Flip( surf );
  }
}

static void 
exec_menu()
{
  SDL_Quit();
# if defined(LINUX_MODE)
# elif defined(CAANOO_MODE)
  chdir("/usr/gp2x");
  execl("/usr/gp2x/gp2xmenu", "/usr/gp2x/gp2xmenu", NULL);
# endif
}

/*** Preogramme principale ***/
/*****************************/
int main(int narg,char *argv[])
{
  int i;
  char **pTitre=NULL;
  char **pIcon=NULL;
  Sprite Spr;
  eMenu RetM,RetMenu=mMenu;

  // Initialuse les préferences
  InitPref();
#ifdef LINUX
  if(narg>1) strcpy(DefPath,argv[1]);
#endif
 
  // Initilise SDL
  if( SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER|SDL_INIT_AUDIO|SDL_INIT_NOPARACHUTE|SDL_INIT_JOYSTICK) < 0 ) {
    cerr <<"Impossible d'initialiser SDL:"<<SDL_GetError()<<endl;
    exit(-1);
  }
# if defined( CAANOO_MODE )
  SDL_JoystickEventState(SDL_ENABLE);
  psp_sdl_joy = SDL_JoystickOpen(0);
# endif
  // Ferme le programme correctement quant quit
  atexit(exec_menu);
    
  // Teste la resolution video
  sdlVideoInfo=(SDL_VideoInfo*)SDL_GetVideoInfo();

  if(sdlVideoInfo->vfmt->BitsPerPixel==8) {
    cerr <<"Impossible d'utiliser 8bits pour la vidéo !"<<endl;
    exit(-1);
  }
  
  // Demande la resolution Video
#ifndef LINUX
  int vOption;
  if(Pref.FullScreen)   vOption=SDL_SWSURFACE; // Bug accé aux bits pour les cordes
  else  vOption=SDL_SWSURFACE|SDL_DOUBLEBUF;
#else
#ifndef __AMIGAOS4__
  int vOption=SDL_SWSURFACE|SDL_DOUBLEBUF;
#else
  int vOption=SDL_SWSURFACE;
#endif
#endif
  if(Pref.FullScreen) vOption|=SDL_FULLSCREEN;
#ifdef GCWZERO
  sdlRVideo=SDL_SetVideoMode(320,240,16,SDL_HWSURFACE|SDL_TRIPLEBUF);
#else
  sdlRVideo=SDL_SetVideoMode(320,240,16,vOption);
#endif

  if(sdlRVideo==NULL) {
    cerr <<"Impossible de passer dans le mode vidéo 320x240 !"<<endl;
    exit(-1);
  }

  sdlVideo = SDL_CreateRGBSurface(SDL_SWSURFACE,
     800, 600,
     sdlRVideo->format->BitsPerPixel,
     sdlRVideo->format->Rmask,
     sdlRVideo->format->Gmask,
     sdlRVideo->format->Bmask, 0);

  // Change le nom de la fenetre
  SDL_WM_GetCaption(pTitre,pIcon);
  SDL_WM_SetCaption(Titre,NULL);
#ifndef GCWZERO
  SDL_ShowCursor(0); // Masque le curseur
#endif
  SDL_EnableUNICODE(1);

  // Couleur du font d'écran
  FontColor=SDL_MapRGB(sdlVideo->format,128,128,128);

  // Chargement des sprites
  Sons.Init();
  if(LoadSprites()==false) exit(-1);
  if(Niveau.Load()==false) exit(-1);

  Sons.PlayMusic();
  Sourie.InitStart();

  // Initialise l'horloge et le hazard
  HorlogeAvant=Horloge=SDL_GetTicks();
  srand(SDL_GetTicks());

  // Si pas de langues demande la langue
  if(Pref.Langue==-1) MenuPrincipale.SDLMain_Langue();
  
  // Gère les menus
  do {
    switch(RetMenu) {
    case mMenu:
      RetM=MenuPrincipale.SDLMain();
      break;
    case mLangue:
      RetM=MenuPrincipale.SDLMain_Langue();
      break;
    case mOption:
      RetM=MenuPrincipale.SDLMain_Options();
      break;
    case mScoreEdit:
      RetM=MenuPrincipale.SDLMain_Score(true);
      break;
    case mScore:
      RetM=MenuPrincipale.SDLMain_Score();
      break;
    case mMenuSpeed:
      RetM=MenuPrincipale.SDLMain_Speed();
      break;
    case mMenuNiveau:
      RetM=MenuPrincipale.SDLMain_Niveau();
      break;
    case mJeux:
      Sons.LoadMusic(1);
      RetM=Jeu.SDLMain();
      Sons.LoadMusic(0);
      break;
    case mEdit:
      RetM=Edit.SDLMain(0);
      break;
    default:
      RetM=mQuit;
    }
    RetMenu=RetM;
  } while(RetMenu!=mQuit);
  
  // Ferme proprement le programme
  Mix_HaltMusic(); // Arrete la music
  Mix_FreeMusic(Sons.Music); // Efface la music
  
  for(i=0;i<NSprites;i++) { // Efface les sprites
    Sprites[i].Delete();
  }
  delete [] Sprites;

  SauvePref(); // Sauve les preferences

  exit(0);
}
