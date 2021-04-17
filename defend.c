/*
  defendguin.c
  
  by Bill Kendrick
  bill@newbreedsoftware.com
  http://www.newbreedsoftware.com/defendguin/
  
  November 6, 1999 - July 28, 2002
*/

#define VERSION "0.0.10"
#define SLOWDOWN 2
/* #define SHOW_FPS */
#define STARTING_LEVEL 1


/* Includes: */

#include <pspsdk.h>
#include <pspkernel.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <math.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

//#ifdef SOUND
#include <SDL/SDL_mixer.h>
//#endif /* #ifdef SOUND */

PSP_MODULE_INFO("defend_psp", 1, 1, 1);

/* Exit callback */
int exit_callback(int arg1, int arg2, void *common)
{
	sceKernelExitGame();

	return 0;
}

/* Callback thread */
int CallbackThread(SceSize args, void *argp)
{
	int cbid;

	cbid = sceKernelCreateCallback("Exit Callback", (void *) exit_callback, NULL);
	sceKernelRegisterExitCallback(cbid);

	sceKernelSleepThreadCB();

	return 0;
}

/* Sets up the callback thread and returns its thread id */
int SetupCallbacks(void)
{
	int thid = 0;

	thid = sceKernelCreateThread("update_thread", CallbackThread, 0x11, 0xFA0, 0, 0);
	if(thid >= 0)
	{
		sceKernelStartThread(thid, 0, 0);
	}

	return thid;
}

#ifndef M_PI
#define M_PI  3.14159265358979323846  /* mmm... pi... */
#endif /* #ifndef M_PI */

//#ifdef GP32
extern SDL_RWops * get_data(const char *name);
//#endif

typedef struct {
	Uint32 points;
	Uint16 level;
	Uint8 music;
	Uint8 sound;
} SaveDataType;

static SaveDataType SaveData = { 1000, 1, 5, 4 };

#define DATA_PREFIX "defend/"

static char *DataFile= DATA_PREFIX "defend.dat";

static void loadData(void)
{

	printf("DataFile = %s\n",DataFile);

	FILE *f=fopen(DataFile,"r");
	if (f!=NULL)
	{
		fread((void *)&SaveData,1,sizeof(SaveDataType),f);
		fclose(f);
	}
}

static void saveData(void)
{
	FILE *f=fopen(DataFile,"w");
	if (f!=NULL)
	{
		fwrite((void *)&SaveData,1,sizeof(SaveDataType),f);
		fclose(f);
	}
}

/* Image enumerations: */

enum {
  IMG_LOADING,
  IMG_LOADING_TEXT,
  IMG_PRESS_A_KEY,
  IMG_TITLE_TITLE,
  IMG_TITLE_ONE_PLAYER,
  IMG_TITLE_TWO_PLAYERS,
  IMG_TITLE_OPTIONS,
  IMG_TITLE_QUIT,
  IMG_TITLE_ARROW0,
  IMG_TITLE_ARROW1,
  IMG_TITLE_ARROW2,
  IMG_TITLE_STARFIELD,
  IMG_TUX_L0,
  IMG_TUX_L1,
  IMG_TUX_L2,
  IMG_TUX_L3,
  IMG_TUX_L4,
  IMG_TUX_L5,
  IMG_TUX_L6,
  IMG_TUX_L7,
  IMG_TUX_R0,
  IMG_TUX_R1,
  IMG_TUX_R2,
  IMG_TUX_R3,
  IMG_TUX_R4,
  IMG_TUX_R5,
  IMG_TUX_R6,
  IMG_TUX_R7,
  IMG_TUX_FALL0,
  IMG_TUX_FALL1,
  IMG_TUX_LFLAG,
  IMG_TUX_RFLAG,
  IMG_UFO0,
  IMG_UFO1,
  IMG_BEAM0,
  IMG_BEAM1,
  IMG_BEAM2,
  IMG_BEAM3,
  IMG_MUTANT0,
  IMG_MUTANT1,
  IMG_POD0,
  IMG_POD1,
  IMG_POD2,
  IMG_SWARMER,
  IMG_BOMBER,
  IMG_BOMBER_BULGE0,
  IMG_BOMBER_BULGE1,
  IMG_MINE,
  IMG_MINE_FLASH,
  IMG_MINE_POP,
  IMG_BAITER0,
  IMG_BAITER1,
  IMG_BAITER2,
  IMG_BAITER3,
  IMG_BAITER4,
  IMG_EVILBILL1,
  IMG_EVILBILL2,
  IMG_EVILBILL_SHOOT,
  IMG_EVILBILL_HURT1,
  IMG_EVILBILL_HURT2,
  IMG_EVILBILL_FLAME1,
  IMG_EVILBILL_FLAME2,
  IMG_EVILBILL_FLAME3,
  IMG_BULLET0,
  IMG_BULLET1,
  IMG_BULLET2,
  IMG_BULLET3,
  IMG_BULLET4,
  IMG_LAND_LEFT,
  IMG_LAND_CENTER,
  IMG_LAND_RIGHT,
  IMG_SHIP_LEFT,
  IMG_SHIP_RIGHT,
  IMG_SHIP_LEFT2,
  IMG_SHIP_RIGHT2,
  IMG_SHIP_LEFT_DYING,
  IMG_SHIP_RIGHT_DYING,
  IMG_SHIP_WIN_0,
  IMG_SHIP_WIN_1,
  IMG_SHIP_WIN_2,
  IMG_DISCO_1,
  IMG_DISCO_2,
  IMG_FLAME_LEFT_0,
  IMG_FLAME_LEFT_1,
  IMG_FLAME_RIGHT_0,
  IMG_FLAME_RIGHT_1,
  IMG_LASERS,
  IMG_STATUS_AREA,
  IMG_MAP_LAND,
  IMG_MAP_PENG,
  IMG_MAP_ALERT,
  IMG_MAP_UFO,
  IMG_MAP_MUTANT,
  IMG_MAP_BOMBER,
  IMG_MAP_MINE,
  IMG_MAP_POD,
  IMG_MAP_SWARMER,
  IMG_MAP_BAITER,
  IMG_MAP_EVILBILL,
  IMG_MAP_SHIP,
  IMG_SHIPS,
  IMG_SMART_BOMB,
  IMG_GET_READY,
  IMG_PENGUINOID_IN_TROUBLE,
  IMG_PENGUINOID_MUTATED,
  IMG_CATCH_THE_PENGUINOID,
  IMG_PENGUINOID_DROPPED,
  IMG_PENGUINOID_SAVED,
  IMG_PENGUINOID_SHOT,
  IMG_PLANET_DESTROYED,
  IMG_LEVEL_BONUS,
  IMG_NO_BONUS,
  IMG_GAME_OVER,
  IMG_NUMBERS,
  IMG_100,
  IMG_150,
  IMG_200,
  IMG_250,
  IMG_1000,
  IMG_CIRCLE,
  IMG_OPTION_TEXT,
  NUM_IMAGES
};

#ifndef FILE_SEPARATOR
#define FILE_SEPARATOR "/"
#endif

/* Image filenames: */

#define DATA_IMAGES DATA_PREFIX "images" FILE_SEPARATOR
#define DATA_LOADER DATA_IMAGES "loader" FILE_SEPARATOR
#define DATA_BAITER DATA_IMAGES "baiter" FILE_SEPARATOR
#define DATA_BOMBER DATA_IMAGES "bomber" FILE_SEPARATOR
#define DATA_TITLE DATA_IMAGES "title" FILE_SEPARATOR
#define DATA_STATUS DATA_IMAGES "status" FILE_SEPARATOR
#define DATA_BULLET DATA_IMAGES "bullet" FILE_SEPARATOR
#define DATA_LAND DATA_IMAGES "land" FILE_SEPARATOR
#define DATA_MUTANT DATA_IMAGES "mutant" FILE_SEPARATOR
#define DATA_POD DATA_IMAGES "pod" FILE_SEPARATOR
#define DATA_POINTS DATA_IMAGES "points" FILE_SEPARATOR
#define DATA_SHIP DATA_IMAGES "ship" FILE_SEPARATOR
#define DATA_SWARMER DATA_IMAGES "swarmer" FILE_SEPARATOR
#define DATA_TUX DATA_IMAGES "tux" FILE_SEPARATOR
#define DATA_UFO DATA_IMAGES "ufo" FILE_SEPARATOR
#define DATA_EVILBILL DATA_IMAGES "evilbill" FILE_SEPARATOR

const char * image_names[NUM_IMAGES] = {
  DATA_LOADER "loading.pcx",
  DATA_LOADER "loadingt.pcx",
  DATA_LOADER "pressak.pcx",
  DATA_TITLE "title.pcx",
  DATA_TITLE "player1.pcx",
  DATA_TITLE "player2.pcx",
  DATA_TITLE "options.pcx",
  DATA_TITLE "quit.pcx",
  DATA_TITLE "arrow0.pcx",
  DATA_TITLE "arrow1.pcx",
  DATA_TITLE "arrow2.pcx",
  DATA_TITLE "starf.pcx",
  DATA_TUX "l0.pcx",
  DATA_TUX "l1.pcx",
  DATA_TUX "l2.pcx",
  DATA_TUX "l3.pcx",
  DATA_TUX "l4.pcx",
  DATA_TUX "l5.pcx",
  DATA_TUX "l6.pcx",
  DATA_TUX "l7.pcx",
  DATA_TUX "r0.pcx",
  DATA_TUX "r1.pcx",
  DATA_TUX "r2.pcx",
  DATA_TUX "r3.pcx",
  DATA_TUX "r4.pcx",
  DATA_TUX "r5.pcx",
  DATA_TUX "r6.pcx",
  DATA_TUX "r7.pcx",
  DATA_TUX "fall0.pcx",
  DATA_TUX "fall1.pcx",
  DATA_TUX "lflag.pcx",
  DATA_TUX "rflag.pcx",
  DATA_UFO "ufo0.pcx",
  DATA_UFO "ufo1.pcx",
  DATA_UFO "beam0.pcx",
  DATA_UFO "beam1.pcx",
  DATA_UFO "beam2.pcx",
  DATA_UFO "beam3.pcx",
  DATA_MUTANT "mutant0.pcx",
  DATA_MUTANT "mutant1.pcx",
  DATA_POD "pod0.pcx",
  DATA_POD "pod1.pcx",
  DATA_POD "pod2.pcx",
  DATA_SWARMER "swarmer.pcx",
  DATA_BOMBER "bomber.pcx",
  DATA_BOMBER "bombb0.pcx",
  DATA_BOMBER "bombb1.pcx",
  DATA_BOMBER "mine.pcx",
  DATA_BOMBER "minef.pcx",
  DATA_BOMBER "minep.pcx",
  DATA_BAITER "baiter0.pcx",
  DATA_BAITER "baiter1.pcx",
  DATA_BAITER "baiter2.pcx",
  DATA_BAITER "baiter3.pcx",
  DATA_BAITER "baiter4.pcx",
  DATA_EVILBILL "eb1.pcx",
  DATA_EVILBILL "eb2.pcx",
  DATA_EVILBILL "ebs.pcx",
  DATA_EVILBILL "ebh1.pcx",
  DATA_EVILBILL "ebh2.pcx",
  DATA_EVILBILL "flame1.pcx",
  DATA_EVILBILL "flame2.pcx",
  DATA_EVILBILL "flame3.pcx",
  DATA_BULLET "bullet0.pcx",
  DATA_BULLET "bullet1.pcx",
  DATA_BULLET "bullet2.pcx",
  DATA_BULLET "bullet3.pcx",
  DATA_BULLET "bullet4.pcx",
  DATA_LAND "left.pcx",
  DATA_LAND "center.pcx",
  DATA_LAND "right.pcx",
  DATA_SHIP "shipl.pcx",
  DATA_SHIP "shipr.pcx",
  DATA_SHIP "shipl2.pcx",
  DATA_SHIP "shipr2.pcx",
  DATA_SHIP "shipld.pcx",
  DATA_SHIP "shiprd.pcx",
  DATA_SHIP "shipw0.pcx",
  DATA_SHIP "shipw1.pcx",
  DATA_SHIP "shipw2.pcx",
  DATA_SHIP "disco1.pcx",
  DATA_SHIP "disco2.pcx",
  DATA_SHIP "flamel0.pcx",
  DATA_SHIP "flamel1.pcx",
  DATA_SHIP "flamer0.pcx",
  DATA_SHIP "flamer1.pcx",
  DATA_SHIP "lasers.pcx",
  DATA_STATUS "status.pcx",
  DATA_STATUS "mapland.pcx",
  DATA_STATUS "mappeng.pcx",
  DATA_STATUS "mapalert.pcx",
  DATA_STATUS "mapufo.pcx",
  DATA_STATUS "mapmut.pcx",
  DATA_STATUS "mapbomb.pcx",
  DATA_STATUS "mapmine.pcx",
  DATA_STATUS "mappod.pcx",
  DATA_STATUS "mapswar.pcx",
  DATA_STATUS "mapbait.pcx",
  DATA_STATUS "mapeb.pcx",
  DATA_STATUS "mapship.pcx",
  DATA_STATUS "ships.pcx",
  DATA_STATUS "smart.pcx",
  DATA_STATUS "getready.pcx",
  DATA_STATUS "trouble.pcx",
  DATA_STATUS "mutated.pcx",
  DATA_STATUS "catch.pcx",
  DATA_STATUS "dropped.pcx",
  DATA_STATUS "saved.pcx",
  DATA_STATUS "shot.pcx",
  DATA_STATUS "planet.pcx",
  DATA_STATUS "levelbon.pcx",
  DATA_STATUS "nobonus.pcx",
  DATA_STATUS "gameover.pcx",
  DATA_STATUS "numbers.pcx",
  DATA_POINTS "100.pcx",
  DATA_POINTS "150.pcx",
  DATA_POINTS "200.pcx",
  DATA_POINTS "250.pcx",
  DATA_POINTS "1000.pcx",
  DATA_STATUS "circle.pcx",
  DATA_STATUS "optext.pcx"
};


/* Sound enumerations: */

enum {
  SND_SELECT,
  SND_CONFIRM,
  SND_IMPATIENCE,
  SND_THRUST,
  SND_LASER,
  SND_SMARTBOMB,
  SND_ONEUP,
  SND_DIE1,
  SND_DIE2,
  SND_MATERIALIZE,
  SND_PENG_CAPTURE,
  SND_PENG_SAVE,
  SND_PENG_DROP0,
  SND_PENG_DROP1,
  SND_PENG_DROP2,
  SND_PENG_KILL,
  SND_PENG_SPLAT,
  SND_PENG_MUTATE,
  SND_EXPLODE,
  SND_SWARMERS,
  SND_BAITER,
  SND_BULLET,
  SND_EVILBILL_INTRO,
  SND_EVILBILL_SUCCESS1,
  SND_EVILBILL_SUCCESS2,
  SND_EVILBILL_SUCCESS3,
  SND_EVILBILL_SHOOT,
  SND_EVILBILL_MINE,
  SND_EVILBILL_HURT,
  SND_EVILBILL_LOW,
  SND_EVILBILL_DIE,
  SND_PLANET_DEATH,
  SND_GAMEOVER0,
  SND_GAMEOVER1,
  SND_GAMEOVER2,
  NUM_SOUNDS
};


/* Sound filenames: */

const char * sound_names[NUM_SOUNDS] = {
  DATA_PREFIX "pp.dat",
  DATA_PREFIX "oo.dat",
  DATA_PREFIX "qq.dat",
  DATA_PREFIX "ll.dat",
  DATA_PREFIX "kk.dat",
  DATA_PREFIX "nn.dat",
  DATA_PREFIX "mm.dat",
  DATA_PREFIX "ii.dat",
  DATA_PREFIX "jj.dat",
  DATA_PREFIX "l.dat",
  DATA_PREFIX "aa.dat",
  DATA_PREFIX "gg.dat",
  DATA_PREFIX "bb.dat",
  DATA_PREFIX "cc.dat",
  DATA_PREFIX "dd.dat",
  DATA_PREFIX "ee.dat",
  DATA_PREFIX "hh.dat",
  DATA_PREFIX "ff.dat",
  DATA_PREFIX "k.dat",
  DATA_PREFIX "n.dat",
  DATA_PREFIX "i.dat",
  DATA_PREFIX "j.dat",
  DATA_PREFIX "q.dat",
  DATA_PREFIX "u.dat",
  DATA_PREFIX "v.dat",
  DATA_PREFIX "w.dat",
  DATA_PREFIX "t.dat",
  DATA_PREFIX "s.dat",
  DATA_PREFIX "p.dat",
  DATA_PREFIX "r.dat",
  DATA_PREFIX "o.dat",
  DATA_PREFIX "m.dat",
  DATA_PREFIX "x.dat",
  DATA_PREFIX "y.dat",
  DATA_PREFIX "z.dat"
};

//#ifdef MUSIC

#define MUS_TITLE DATA_PREFIX "f.dat"
#define MUS_LAST DATA_PREFIX "g.dat"
#define MUS_WIN DATA_PREFIX "h.dat"

#define NUM_GAME_MUSICS 5

const char * game_music_names[NUM_GAME_MUSICS] = {
  DATA_PREFIX "b.dat",
  DATA_PREFIX "c.dat",
  DATA_PREFIX "a.dat",
  DATA_PREFIX "d.dat",
  DATA_PREFIX "e.dat"
};
//#endif

#define NUM_QUOTES 9

const char * quotes[NUM_QUOTES] = {
  "ABOUT TO EXPLODE",
  "ARE YOU READY",
  "JUST PRESS THE RIGHT BUTTON",
  "NO   LET ME FINISH",
  "DARN  THATS THE END",
  "I DONT LIKE TO LOSE",
  "WRITTEN IN EMACS",
  "I DONT PUT ANY RESTRICTIONS",
  "ACHEM"
};


/* Ship directions: */

enum {
  DIR_LEFT,
  DIR_RIGHT
};


/* Penguinoid modes: */

enum {
  PENG_MODE_WALKING,
  PENG_MODE_BEAMING,
  PENG_MODE_CAPTURED,
  PENG_MODE_FALLING,
  PENG_MODE_SAVED
};


/* Alien types: */

enum {
  ALIEN_UFO,
  ALIEN_MUTANT,
  ALIEN_BOMBER,
  ALIEN_MINE,
  ALIEN_POD,
  ALIEN_SWARMER,
  ALIEN_BAITER,
  ALIEN_EVILBILL
};


/* Alien modes: */

enum {
  ALIEN_MODE_BEAM_IN,
  ALIEN_MODE_NORMAL,
  ALIEN_MODE_HOME_PENG,
  ALIEN_MODE_BEAMING_PENG,
  ALIEN_MODE_CAPTURED_PENG,
  ALIEN_MODE_HOME_SHIP,
  ALIEN_MODE_SHOOTING,
  ALIEN_MODE_HURTING,
  ALIEN_MODE_DYING
};


/* Title Options: */

enum {
  TITLE_OPTION_ONE_PLAYER,
  TITLE_OPTION_TWO_PLAYERS,
  TITLE_OPTION_OPTIONS,
  TITLE_OPTION_QUIT,
  NUM_TITLE_OPTIONS
};


/* Title option images: */

const int title_option_images[NUM_TITLE_OPTIONS] = {
  IMG_TITLE_ONE_PLAYER,
  IMG_TITLE_TWO_PLAYERS,
  IMG_TITLE_OPTIONS,
  IMG_TITLE_QUIT
};


/* Arbitrary constraints: */

#define MAX_ALIENS 32
#define MAX_PENGUINOIDS 16
#define MAX_BULLETS 64
#define MAX_LASERS 16
#define MAX_STARS 64 
#define MAX_STARFIELDS 16
#define MAX_EXPLOSION_BITS 2048
#define MAX_POINTS 16
#define MAX_FLAMES 8

#define LASER_SPEED 128

#define LAND_WIDTH 70
#define LAND_HEIGHT 15
#define LAND_MIN_HEIGHT 10

#define CHANCE_UFO_HOME_PENG 1000
#define CHANCE_UFO_SHOOT_NORMAL 150
#define CHANCE_UFO_SHOOT_HOME 20
#define CHANCE_MUTANT_SHOOT 100
#define CHANCE_SWARMER_SHOOT 100
#define CHANCE_BAITER_SHOOT 50
#define CHANCE_EVILBILL_SHOOT 20
#define CHANCE_EVILBILL_MINE 100


/* Joystick defaults: */

//#ifdef JOY_YES
#define JOY_A 0
#define JOY_B 1
#define JOY_X 0
#define JOY_Y 1
//#endif /* #ifdef JOY_YES */


/* Typedefs: */

typedef struct alien_type {
  int alive, type, mode, timer, shields;
  int x, y, xm, ym, xmm, ymm;
  int home_peng, beam_height;
} alien_type;

typedef struct bullet_type {
  int alive, timer, owner;
  int x, y, xm, ym;
} bullet_type;

typedef struct penguinoid_type {
  int alive, mode, being_homed;
  int x, y, xm, ym;
} penguinoid_type;

typedef struct laser_type {
  int alive;
  int x1, x2, y;
} laser_type;

typedef struct star_type {
  int x, y;
} star_type;

typedef struct starfield_type {
  int time;
  float radius;
  int angle;
} starfield_type;

typedef struct explosion_bit_type {
  int alive, time, img;
  int x, y, xm, ym;
} explosion_bit_type;

typedef struct points_type {
  int alive, time, img;
  int x, y;
} points_type;

typedef struct flame_type {
  int alive, time, x, y, xm;
} flame_type;


/* Globals: */

int x[2], y[2], xm[2], scroll[2], dir[2], lives[2], bombs[2], planet_dead[2],
  dying[2], dancing[2], safe[2], level[2];
Uint32 score[2];
int player;
int dontblockchan[4];
float my_cos[32];
int highscore, num_players, vol_effects=4, vol_music=5;
int mesg_img, mesg_timer, flash, flash_colors, oneup_effect,
  oneup_effect_counter;
int use_fullscreen, use_sound, use_joystick, num_joysticks;
SDL_Surface * screen;
SDL_Surface * images[NUM_IMAGES];
int land[2][LAND_WIDTH][LAND_HEIGHT];
alien_type aliens[2][MAX_ALIENS];
penguinoid_type penguinoids[2][MAX_PENGUINOIDS];
bullet_type bullets[MAX_BULLETS];
laser_type lasers[MAX_LASERS];
star_type stars[MAX_STARS];
explosion_bit_type explosion_bits[MAX_EXPLOSION_BITS];
points_type points[MAX_POINTS];
flame_type flames[MAX_FLAMES];
starfield_type starfields[MAX_STARFIELDS];

//#ifdef SOUND
Mix_Chunk * sounds[NUM_SOUNDS];
//#ifdef MUSIC
Mix_Music * title_music, * last_music, * win_music;
Mix_Music * game_musics[NUM_GAME_MUSICS];
//#endif
//#endif /* #ifdef SOUND */

//#ifdef JOY_YES
SDL_Joystick *js;
//#endif /* #ifdef JOY_YES */


/* Local function prototypes: */

int game(int mode);
int option_screen(void);
int title(void);
void setup(void);
void my_shutdown(void);
void create_land(int ply);
void create_penguinoids(int ply, int num);
void draw_number(int x, int y, int n);
void add_alien(int x, int y, int type, int mode, int timer);
void create_stars(void);
void add_laser(int x, int y, int dir);
void playsound(int snd, int chan, int blocking);
void add_explosion(int x, int y, int img);
void add_explosion_bit(int x, int y, int xm, int ym, int img);
void add_points(int xx, int yy, int img);
void add_flame(int x, int y);
void kill_alien(int i);
void add_bullet(int xx, int yy, int owner);
void set_message(int img);
void kill_player(void);
void add_score(int add);
void smartbomb(void);
void usage(int err);
void pause_screen(void);
void write_text(int x, int y, char * str);
void write_text_inv(int x, int y, char * str);
void write_centered_text(int y, char * str);
void write_num(int x, int y, int v);
int option_value(int opt_line);


/* --- MAIN --- */
int main(int argc, char * argv[])
{
  int mode, quit, i;
  pspDebugScreenInit();
	SetupCallbacks();

//#ifdef GP32
//        x_gp32_SetCPUSpeed_133();
//#endif
  
  /* Check for arguments: */
  
  use_fullscreen = 0;
  use_sound = 1;

  /* Setup: */
 
  loadData();
  setup();
  quit = 0;
  
  for (i = 0; i < 2; i++)
    {
      score[i] = -1;
      level[i] = -1;
    }
  
  
  /* MAIN INTERFACE LOOP! */
  
  do
    {
      /* Display the title screen: */
      
      mode = title();
      
      
      /* What's our choice? */
      
      if (mode == TITLE_OPTION_ONE_PLAYER ||
	  mode == TITLE_OPTION_TWO_PLAYERS)
	{
	  /* Play the game */

	  quit = game(mode);
	  if (score[0]>SaveData.points)
		  SaveData.points=score[0];
	  if (score[1]>SaveData.points)
		  SaveData.points=score[1];
	  if (level[0]>SaveData.level)
		  SaveData.level=level[0];
	  if (level[1]>SaveData.level)
		  SaveData.level=level[1];
	  saveData();
	}
      else if (mode == TITLE_OPTION_OPTIONS)
	{
	  /* Option screen: */
	  
	  quit = option_screen();
	  SaveData.music=vol_music;
	  SaveData.sound=vol_effects;
	  saveData();
	}
      else if (mode == TITLE_OPTION_QUIT)
	{
	  quit = 1;
	}
    }
  while (quit == 0);
  

  /* Shut down and quit: */
  
	sceKernelExitGame();
  return(0);
}


/* Game loop: */

int game(int mode)
{
  SDL_Event event;
  SDLKey key;
  SDL_Rect dest, src;
  int i, j, done, quit, frame, zz, tmp_x, tmp_y, img, xsize, ysize,
    left_down, right_down, up_down, down_down, thrust_down,
    num_players, x1, x2, num_penguinoids, num_aliens;
  int level_beat[2], level_time[2];
  Uint32 last_time;
  
  
  /* Stop any music: */
  
//#ifdef SOUND
  if (use_sound == 1)
    Mix_HaltMusic();
//#endif /* #ifdef SOUND */

	  
  /* Set number of players: */
  
  if (mode == TITLE_OPTION_ONE_PLAYER)
    num_players = 1;
  else
    num_players = 2;
  
  
  /* Reset keypress flags: */
  
  left_down = 0;
  right_down = 0;
  up_down = 0;
  down_down = 0;
  thrust_down = 0;
  
  
  /* Reset player stuff: */
  
  for (i = 0; i < 2; i++)
    {
      x[i] = 0;
      xm[i] = 0;
      y[i] = 240;
      safe[i] = 50;
      scroll[i] = 128;
      dir[i] = DIR_RIGHT;
      lives[i] = 3;
      bombs[i] = 3;
      score[i] = 0;
      level[i] = STARTING_LEVEL;
      dying[i] = 0;
      dancing[i] = 0;
      planet_dead[i] = 0;

      create_land(i);
      create_penguinoids(i, MAX_PENGUINOIDS);

      level_time[i] = 0;
      level_beat[i] = 0;
    }
  
  
  /* Create stars: */
  
  create_stars();
  
  
  /* Turn off explosion bits: */
  
  for (i = 0; i < MAX_EXPLOSION_BITS; i++)
    explosion_bits[i].alive = 0;
  
  
  /* Turn off points: */
  
  for (i = 0 ; i < MAX_POINTS; i++)
    points[i].alive = 0;
  
  
  /* Turn off flames: */
  
  for (i = 0 ; i < MAX_FLAMES; i++)
    flames[i].alive = 0;
  
  
  /* Create aliens: */
  
  for (i = 0; i < 2; i++)
    {
      for (j = 0; j < MAX_ALIENS; j++)
	aliens[i][j].alive = 0;
    }
      
  for (i = 0; i < MAX_BULLETS; i++)
    bullets[i].alive = 0;

  
  /* Turn off lasers: */
  
  for (i = 0; i < MAX_LASERS; i++)
    lasers[i].alive = 0;
  
  
  /* Main game loop! */
  
  frame = 0;
  key = SDLK_LAST;
  done = 0;
  quit = 0;
  player = 0;
  flash = 0;
  flash_colors = 0;
  oneup_effect = 0;
  oneup_effect_counter = 0;
  
  
  do
    {

	//sceKernelDelayThread(30000);

      last_time = SDL_GetTicks();
      frame++;
      
      if (level_beat[player] == 0)
	level_time[player]++;
      
      
      while (SDL_PollEvent(&event))
	{
	  if (event.type == SDL_KEYDOWN)
	    {
	      /* A keypress! */
	      
	      key = event.key.keysym.sym;
	      
	      if (key == SDLK_SPACE)
		{
		  /* Escape: QUit the game and return to main menu: */
		  
		  done = 1;
		}
	      else if (key == SDLK_LEFT &&
		       dying[player] == 0 &&
		       dancing[player] == 0)
		{
		  right_down = 0;
		  left_down = 1;
		  
		  if (dir[player] != DIR_LEFT)
		    {
		      dir[player] = DIR_LEFT;
		      /* scroll[player] = scroll[player] + xm[player]; */
		    }
		}
	      else if (key == SDLK_RIGHT &&
		       dying[player] == 0 &&
		       dancing[player] == 0)
		{
		  left_down = 0;
		  right_down = 1;
		  
		  if (dir[player] != DIR_RIGHT)
		    {
		      dir[player] = DIR_RIGHT;
		    }
		}
	      else if (key == SDLK_2 && dying[player] == 0 &&
		       dancing[player] == 0)
		{
		  if (dir[player] == DIR_LEFT)
		    dir[player] = DIR_RIGHT;
		  else
		    dir[player] = DIR_LEFT;
		  
		  left_down = 0;
		  right_down = 0;
		}
	      else if (key == SDLK_1 && dying[player] == 0 &&
		       dancing[player] == 0)
		{
		  thrust_down = 1;
		}
	      else if (key == SDLK_UP)
		{
		  up_down = 1;
		}
	      else if (key == SDLK_DOWN)
		{
		  down_down = 1;
		}
	      else if (key == SDLK_LCTRL && dying[player] == 0 &&
		       dancing[player] == 0)
		{
		  add_laser((xm[player] * 8) + scroll[player],
			    y[player] + 18 + ((rand() % 4) * 2),
			    dir[player]);
		  
		  playsound(SND_LASER, 1, 0);
		}
	      else if (
		       key == SDLK_LALT  &&
		       dying[player] == 0 &&
		       dancing[player] == 0)
		{
		  /* Detonante smart-bomb: */
		  
		  smartbomb();
		}
	      else if (key == SDLK_RETURN)
		{
		  /* Pause: */
		  
		  pause_screen();
		  up_down = 0;
		  down_down = 0;
		  left_down = 0;
		  right_down = 0;
		}
	    }
	  else if (event.type == SDL_KEYUP)
	    {
	      /* A key-release: */
	      
	      key = event.key.keysym.sym;
	      
	      if (key == SDLK_LEFT)
		left_down = 0;
	      else if (key == SDLK_RIGHT)
		right_down = 0;
	      else if (key == SDLK_UP)
		up_down = 0;
	      else if (key == SDLK_DOWN)
		down_down = 0;
	      else if (key == SDLK_1)
		thrust_down = 0;
	    }
//#ifdef JOY_YES
	  else if (event.type == SDL_JOYAXISMOTION &&
		   (num_joysticks < 2 || event.jaxis.which == player))
	    {
	      /* Joystick event: */
	      if (event.jaxis.axis == JOY_X)
		{
		  /* Left/right motion */

		  if (event.jaxis.value < -256 &&
		      dying[player] == 0 &&
		      dancing[player] == 0)
		    {
		      /* Stick left: */
		      
		      right_down = 0;
		      left_down = 1;
		      
		      if (dir[player] != DIR_LEFT)
			{
			  dir[player] = DIR_LEFT;
			}
		    }
		  else
		    left_down = 0;
		  
		  
		  if (event.jaxis.value > 256 &&
		      dying[player] == 0 &&
		      dancing[player] == 0)
		    {
		      /* Stick right: */
		      
		      right_down = 1;
		      left_down = 0;
		      
		      if (dir[player] != DIR_RIGHT)
			{
			  dir[player] = DIR_RIGHT;
			}
		    }
		  else
		    right_down = 0;
		}
	      else if (event.jaxis.axis == JOY_Y)
		{
		  /* Up/down motion: */
		  
		  if (event.jaxis.value > 256)
		    down_down = 1;
		  else
		    down_down = 0;

		  if (event.jaxis.value < -256)
		    up_down = 1;
		  else
		    up_down = 0;
		}
	    }
//#ifndef GP32
	  else if (event.type == SDL_JOYBUTTONDOWN &&
		   (num_joysticks < 2 || event.jbutton.which == player))
	    {
	      if (event.jbutton.button == JOY_B &&
		  dying[player] == 0 &&
		  dancing[player] == 0)
		{
		  /* Fire laser: */

		  add_laser((xm[player] * 8) + scroll[player],
			    y[player] + 18 + ((rand() % 4) * 2),
			    dir[player]);
		  
		  playsound(SND_LASER, 1, 0);
		}
	      else if (event.jbutton.button == JOY_A &&
		       dying[player] == 0 &&
		       dancing[player] == 0)
		{
		  /* Detonante smart-bomb: */

		  smartbomb();
		}
	    }
//#endif
//#endif /* #ifdef JOY_YES */
	  else if (event.type == SDL_QUIT)
	    {
	      quit = 1; 
	    }
	}
      
      
      /* Handle beaming-in new aliens: */
      
      if (level_time[player] == 2)
	{
	  /* First set of aliens: */
	  
	  if (level[player] != 20)
	    {
	      for (i = 0; i < ((level[player] % 5) + 1) * 5; i++)
		add_alien(-1, -1, ALIEN_UFO, ALIEN_MODE_BEAM_IN, 64);
	      
	      for (i = 0; i < ((level[player] - 1) * 2) && i < 8; i++)
		add_alien(-1, -1, ALIEN_BOMBER, ALIEN_MODE_BEAM_IN, 64);
	      
	      for (i = 0; i < ((level[player] / 2)); i++)
		add_alien(-1, -1, ALIEN_POD, ALIEN_MODE_BEAM_IN, 32);
	      
	      playsound(SND_MATERIALIZE, 3, 1);
	    }
	  else
	    {
	      /* Last level! */
	      
	      add_alien(-1, -1, ALIEN_EVILBILL, ALIEN_MODE_NORMAL, 32);
	      playsound(SND_EVILBILL_INTRO, 3, 1);
	    }
	  
	  set_message(IMG_GET_READY);
	}
      
      
      if (level[player] != 20)
	{
	  if (level_time[player] == 1000)
	    {
	      /* Second set of aliens: */
	      
	      for (i = 0; i < 10; i++)
		add_alien(-1, -1, ALIEN_UFO, ALIEN_MODE_BEAM_IN, 64);
	      
	      playsound(SND_MATERIALIZE, 3, 1);
	    }
	  
	  
	  if (level_time[player] >= 1500 && (level_time[player] % 500) == 0)
	    {
	      /* Baiters */
	      
	      add_alien(-1, -1, ALIEN_BAITER, ALIEN_MODE_BEAM_IN, 16);
	      
	      playsound(SND_BAITER, 3, 1);
	    }
	}
      
      
      /* Handle message: */
      
      if (mesg_timer > 0)
	mesg_timer--;


      /* Handle oneup effect timer: */
      
      if (oneup_effect > 0)
	{
	  oneup_effect = oneup_effect - 2;
	  
	  if (oneup_effect == 0)
	    {
	      oneup_effect_counter--;
	      if (oneup_effect_counter > 0)
		oneup_effect = 32;
	    }
	}
      
      
      /* Handle ship thrust: */
      
      if ((left_down == 1 || (thrust_down == 1 &&
			      dir[player] == DIR_LEFT)) &&
	  dying[player] == 0 &&
	  dancing[player] == 0)
	{
	  /* Increase our left speed: */
	  
	  if (xm[player] > -16)
	    xm[player] = xm[player] - 2;
	  
	  if ((frame % 4) == 0)
	    playsound(SND_THRUST, 0, 0);
	}
      else if ((right_down == 1 || (thrust_down == 1 &&
			       dir[player] == DIR_RIGHT)) &&
	       dying[player] == 0 &&
	       dancing[player] == 0)
	{
	  /* Increase our right speed: */
	  
	  if (xm[player] < 16)
	    xm[player] = xm[player] + 2;

	  if ((frame % 4) == 0)
	    playsound(SND_THRUST, 0, 0);
	}
      else
	{
	  /* Slow down if we're not pressing any keys: */
	  
	  if (xm[player] < 0)
	    xm[player]++;
	  else if (xm[player] > 0)
	    xm[player]--;
	  
//#ifdef SOUND
	  if (use_sound == 1)
	    {
	      if (Mix_Playing(0))
		Mix_HaltChannel(0);
	    }
//#endif /* #ifdef SOUND */
	}
      
      
      /* Move ship: */
      
      x[player] = x[player] + xm[player];
      
      if (x[player] < 0)
	x[player] = x[player] + (LAND_WIDTH * 32);
      else if (x[player] >= (LAND_WIDTH * 32))
	x[player] = x[player] - (LAND_WIDTH * 32);
      
      if (dying[player] == 0 &&
	  dancing[player] == 0)
	{
	  if (up_down == 1 && y[player] > 80)
	    y[player] = y[player] - 16;
	  else if (down_down == 1 && y[player] < 448)
	    y[player] = y[player] + 16;
	}
      
      
      if (dancing[player] && y[player] < 400)
	y[player]++;
      
      
      /* Handle ship death: */
      
      if (dying[player] > 0)
	{
	  dying[player]++;
	  
	  
	  /* End of white flash?  Add some explosion bits! */
	  
	  if (dying[player] == 20)
	    {
	      for (i = 0; i < 64; i = i + 4)
		{
		  for (j = 0; j < 32; j = j + 4)
		    {
		      add_explosion_bit(x[player] + xm[player] + i,
					y[player] + j,
					(rand() % 8) - 4,
					(rand() % 8) - 4,
					IMG_LASERS);
		    }
		}
	    }
	  
	  
	  /* Come back to life: */
	  
	  if (dying[player] == 100)
	    {
	      lives[player]--;
	      
	      if (num_players == 2)
		{
		  player = 1 - player;
		  
		  if (lives[player] < 0)
		    player = 1 - player;
		}
	      
	      if (lives[player] >= 0)
		{
		  dying[player] = 0;
		  x[player] = (rand() % (LAND_WIDTH * 32));
		  y[player] = 240;
		  xm[player] = 0;
		  dir[player] = DIR_RIGHT;
		  scroll[player] = 128;
		  safe[player] = 50;
		}
	    }
	}
      
      
      /* Handle dancing ship: */
      
      if (dancing[player] > 0)
	{
	  dancing[player]++;
	  
	  if (dancing[player] == 1000)
	    done = 1;
	}
      
      
      /* Handle lasers: */
      
      for (i = 0; i < MAX_LASERS; i++)
	{
	  if (lasers[i].alive)
	    {
	      /* Shoot towards the endpoint, and turn off when we hit it: */
	      
	      if (lasers[i].x2 == 640)
		{
		  lasers[i].x1 = lasers[i].x1 + LASER_SPEED;
		  if (lasers[i].x1 >= 640)
		    lasers[i].alive = 0;
		}
	      else if (lasers[i].x2 == 0)
		{
		  lasers[i].x1 = lasers[i].x1 - LASER_SPEED;
		  if (lasers[i].x1 <= 0)
		    lasers[i].alive = 0;
		}
	    }
	}


      /* Handle bullets: */
      
      for (i = 0; i < MAX_BULLETS; i++)
	{
	  if (bullets[i].alive)
	    {
	      /* Move: */
	      
	      bullets[i].x = bullets[i].x + bullets[i].xm;
	      bullets[i].y = bullets[i].y + bullets[i].ym;
	      
	      
	      /* Wrap around: */
	      
	      if (bullets[i].x < 0)
		bullets[i].x = bullets[i].x + (LAND_WIDTH * 32);
	      else if (bullets[i].x > LAND_WIDTH * 32)
		bullets[i].x = bullets[i].x - (LAND_WIDTH * 32);
	      
	      
	      /* Count down: */
	      
	      bullets[i].timer--;
	      
	      
	      /* Die? */
	      
	      if (bullets[i].y < 80 || bullets[i].y >= 480 ||
		  bullets[i].timer <= 0)
		bullets[i].alive = 0;
	      
	      
	      /* Did we hit the player? */
	      
	      if (safe[player] == 0 && dying[player] == 0 &&
		  dancing[player] == 0 &&
		  bullets[i].x >= x[player] + (xm[player] * 8) &&
		  bullets[i].x <= x[player] + (xm[player] * 8) + 64 &&
		  bullets[i].y >= y[player] - 16 &&
		  bullets[i].y <= y[player] + 16)
		{
		  /* Kill the bullet: */
		  
		  bullets[i].alive = 0;
		  
		  
		  /* Kill the player: */
		  
		  kill_player();
		  
		  
		  /* Evil Bill says stuff: */
		  
		  if (aliens[player][bullets[i].owner].type == ALIEN_EVILBILL)
		    playsound(SND_EVILBILL_SUCCESS1 + (rand() % 3), 1, 1);
		}
	    }
	}
      
      
      /* Hanlde points: */
      
      for (i = 0; i < MAX_POINTS; i++)
	{
	  if (points[i].alive)
	    {
	      /* Move up and count down: */
	      
	      points[i].y = points[i].y - 2;
	      points[i].time--;
	      

	      /* If we've counted all the way down (or hit the top of
		 the screen), then go away: */
	      
	      if (points[i].y <= 0 || points[i].time <= 0)
		points[i].alive = 0;
	    }
	}
      
      
      /* Hanlde flames: */
      
      for (i = 0; i < MAX_FLAMES; i++)
	{
	  if (flames[i].alive)
	    {
	      /* Move up and count down: */
	      
	      flames[i].y = flames[i].y - 4;
	      flames[i].x = flames[i].x + flames[i].xm;
	      flames[i].time--;
	      

	      /* If we've counted all the way down (or hit the top of
		 the screen), then go away: */
	      
	      if (flames[i].y <= 0 || flames[i].time <= 0)
		flames[i].alive = 0;
	    }
	}
      
      
      /* Handle explosion bits: */
      
      for (i = 0; i < MAX_EXPLOSION_BITS; i++)
	{
	  if (explosion_bits[i].alive)
	    {
	      /* Move the bit: */
	      
	      explosion_bits[i].x = explosion_bits[i].x + explosion_bits[i].xm;
	      explosion_bits[i].y = explosion_bits[i].y + explosion_bits[i].ym;
	      
	      
	      /* Keep in bounds: */
	      
	      if (explosion_bits[i].x < 0)
		explosion_bits[i].x = explosion_bits[i].x + (LAND_WIDTH * 32);
	      else if (explosion_bits[i].x > LAND_WIDTH * 32)
		explosion_bits[i].x = explosion_bits[i].x - (LAND_WIDTH * 32);
	      
	      if (explosion_bits[i].y < 80 || explosion_bits[i].y >= 480)
		explosion_bits[i].alive = 0;
	      
	      
	      /* Count time down: */
	      
	      explosion_bits[i].time--;
	      if (explosion_bits[i].time <= 0)
		explosion_bits[i].alive = 0;
	    }
	}
      
      
      /* Move penguinoids: */
      
      num_penguinoids = 0;
      
      for (i = 0; i < MAX_PENGUINOIDS; i++)
	{
	  if (penguinoids[player][i].alive)
	    {
	      if (level[player] != 20)
		{
		  if (penguinoids[player][i].mode == PENG_MODE_WALKING)
		    {
		      /* Move horizontally: */
		      
		      penguinoids[player][i].x =
			penguinoids[player][i].x + penguinoids[player][i].xm;
		      
		      if (penguinoids[player][i].x < 0)
			penguinoids[player][i].x = (penguinoids[player][i].x +
					    (LAND_WIDTH * 32));
		      else if (penguinoids[player][i].x > LAND_WIDTH * 32)
			penguinoids[player][i].x = (penguinoids[player][i].x -
					    (LAND_WIDTH * 32));
		      
		      
		      /* Move vertically: */
		      
		      penguinoids[player][i].y =
			penguinoids[player][i].y + penguinoids[player][i].ym;
		      
		      if (penguinoids[player][i].y > 448)
			{
			  penguinoids[player][i].y = 448;
			  penguinoids[player][i].ym = -((rand() % 2) + 1);
			}
		      else if (land[player][penguinoids[player][i].x / 32]
			       [penguinoids[player][i].y / 32]
			       == -1)
			{
			  penguinoids[player][i].y =
			    penguinoids[player][i].y + 1;
			  
			  penguinoids[player][i].ym = (rand() % 2) + 1;
			}
		    }
		  else if (penguinoids[player][i].mode == PENG_MODE_FALLING)
		    {
		      /* Move vertically: */
		      
		      penguinoids[player][i].y = (penguinoids[player][i].y +
						  penguinoids[player][i].ym);
		      
		      if ((frame % 2) == 0)
			{
			  penguinoids[player][i].ym =
			    penguinoids[player][i].ym + 1;
			}
		      
		      if (penguinoids[player][i].ym > 32)
			penguinoids[player][i].ym = 32;
		      
		      
		      /* Land or splat: */

		      
		      if (land[player][penguinoids[player][i].x / 32]
			  [penguinoids[player][i].y / 32] != -1)
			{
			  if (penguinoids[player][i].ym <= 10)
			    {
			      /* Land: */
			      
			      penguinoids[player][i].ym = 0;
			      penguinoids[player][i].mode = PENG_MODE_WALKING;
			      penguinoids[player][i].being_homed = 0;
			      add_score(100);
			      add_points(penguinoids[player][i].x,
					 penguinoids[player][i].y,
					 IMG_100);
			      set_message(IMG_PENGUINOID_SAVED);
			    }
			  else
			    {
			      /* Splat! */
			      
			      penguinoids[player][i].alive = 0;
			      flash = 1;
			      playsound(SND_PENG_SPLAT, 2, 0);
			      add_explosion(penguinoids[player][i].x + 16,
					    penguinoids[player][i].y + 16,
					    IMG_TUX_FALL0);
			      set_message(IMG_PENGUINOID_DROPPED);
			    }
			}
		      
		      
		      /* Get saved by the ship!? */
		      
		      if (penguinoids[player][i].x >= (x[player] +
					       (xm[player] * 8) - 32) &&
			  penguinoids[player][i].x <= (x[player] +
					       (xm[player] * 8) + 64) &&
			  penguinoids[player][i].y >= y[player] - 32 &&
			  penguinoids[player][i].y <= y[player] + 32 &&
			  dying[player] == 0 &&
			  dancing[player] == 0)
			{
			  penguinoids[player][i].mode = PENG_MODE_SAVED;
			  penguinoids[player][i].being_homed = 0;
			  playsound(SND_PENG_SAVE, 2, 0);
			  add_score(100);
			  add_points(penguinoids[player][i].x,
				     penguinoids[player][i].y,
				     IMG_100);
			  set_message(IMG_PENGUINOID_SAVED);
			}
		    }
		  else if (penguinoids[player][i].mode == PENG_MODE_SAVED)
		    {
		      /* Stick to the ship! */
		      
		      penguinoids[player][i].x = (x[player] +
						  (xm[player] * 8) + 16);
		      penguinoids[player][i].y = y[player] + 32;
		      
		      
		      /* Stick to the ground if we brush against it: */
		      
		      if (land[player][penguinoids[player][i].x / 32]
			  [penguinoids[player][i].y / 32] != -1)
			{
			  penguinoids[player][i].ym = 0;
			  penguinoids[player][i].mode = PENG_MODE_WALKING;
			  penguinoids[player][i].being_homed = 0;
			  playsound(SND_PENG_DROP0 + (rand() % 3), 2, 1);
			  add_score(250);
			  add_points(penguinoids[player][i].x,
				     penguinoids[player][i].y,
				     IMG_250);
			  set_message(IMG_PENGUINOID_SAVED);
			}
		    }
		}
	      else
		{
		  /* Last level!  Cheering! */
		  
		  if (num_aliens == 0)
		    {
		      /* No more EvilBill? Jump up and down! */
		      
		      penguinoids[player][i].y = (penguinoids[player][i].y +
						  penguinoids[player][i].ym);
		      penguinoids[player][i].ym++;
		      
		      if (penguinoids[player][i].ym >= 5)
			{
			  penguinoids[player][i].ym = 0;
			  penguinoids[player][i].y =
			    penguinoids[player][i].y - 10;
			}
		      
		      
		      /* Move towards ship if it's landed: */
		      
		      if (dancing[player] && y[player] == 400)
			{
			  if (penguinoids[player][i].x < x[player] -
			      ((i + 3) * 14))
			    {
			      penguinoids[player][i].x =
				penguinoids[player][i].x + 4;
			      penguinoids[player][i].xm = 1;
			    }
			  else if (penguinoids[player][i].x > x[player] +
				   ((i + 3) * 14))
			    {
			      penguinoids[player][i].x =
				penguinoids[player][i].x - 4;
			      penguinoids[player][i].xm = -1;
			    }
			}
		    }
		}
	      
	      
	      /* Kill penguinoids if shot by our laser: */
	      
	      /* (determine relative position on screen (laser
		 is screenbased)) */
	      
	      tmp_x = penguinoids[player][i].x - (x[player] - scroll[player]);
	      
	      if (tmp_x < -31)
		tmp_x = tmp_x + (LAND_WIDTH * 32);
	      
	      while (tmp_x >= (LAND_WIDTH * 32))
		tmp_x = tmp_x - (LAND_WIDTH * 32);
	      
	      
	      /* (see if any lasers are touching the penguinoid) */
	      
	      for (j = 0; j < MAX_LASERS; j++)
		{
		  if (lasers[j].alive && penguinoids[player][i].alive)
		    {
		      if (((lasers[j].x2 == 640 &&
			    tmp_x >= lasers[j].x1 && tmp_x <= 640) ||
			   (lasers[j].x2 == 0 &&
			    tmp_x <= lasers[j].x1 && tmp_x >= 0)) &&
			  (penguinoids[player][i].y >= lasers[j].y - 32 &&
			   penguinoids[player][i].y <= lasers[j].y))
			{
			  /* Kill the penguinoid: */
			  
			  penguinoids[player][i].alive = 0;
			  flash = 1;
			  playsound(SND_PENG_KILL, 2, 0);
			  set_message(IMG_PENGUINOID_SHOT);

			  add_explosion(penguinoids[player][i].x + 16,
					penguinoids[player][i].y + 16,
					IMG_TUX_FALL0);
			}
		    }
		}
	      
	      num_penguinoids++;
	    }
	}
      
      
      /* Are there no more penguinoids? */
      
      if (num_penguinoids == 0 && planet_dead[player] == 0)
	{
	  /* Kill the planet! */
	  
	  playsound(SND_PLANET_DEATH, 3, 1);
	  set_message(IMG_PLANET_DESTROYED);
	  
	  for (i = 0; i < LAND_WIDTH; i++)
	    {
	      for (j = 0; j < LAND_HEIGHT; j++)
		{
		  if (land[player][i][j] != -1)
		    {
		      add_explosion(i * 32 + 16, j * 32 + 16,
				    IMG_LASERS);
		      land[player][i][j] = -1;
		    }
		}
	    }
	  
	  planet_dead[player] = 1;
	  
	  flash = 20;
	  
	  
	  /* Turn all aliens (except mines & evilbill) into mutants: */
	  
	  for (i = 0; i < MAX_ALIENS; i++)
	    {
	      if (aliens[player][i].alive)
		{
		  if (aliens[player][i].type != ALIEN_MINE)
		    {
		      if (aliens[player][i].type != ALIEN_EVILBILL)
			aliens[player][i].type = ALIEN_MUTANT;
		    }
		  else
		    aliens[player][i].alive = 0;
		}
	    }
	}
      
      
      /* Move aliens: */
      
      num_aliens = 0;
      
      for (i = 0; i < MAX_ALIENS; i++)
	{
	  if (aliens[player][i].alive)
	    {
	      xsize = 0;
	      if (aliens[player][i].type == ALIEN_EVILBILL)
		xsize = 128 - 32;
	      
	      ysize = 0;
	      if (aliens[player][i].type == ALIEN_EVILBILL)
		ysize = 128 - 32;
	      
	      num_aliens++;
	      
	      if (aliens[player][i].type == ALIEN_UFO)
		{
		  /* UFO's! */
		  
		  if (aliens[player][i].mode == ALIEN_MODE_NORMAL)
		    {
		      /* Move horizontally: */
		      
		      aliens[player][i].x = (aliens[player][i].x +
					     aliens[player][i].xm);
		      
		      if (aliens[player][i].x < 0)
			{
			  aliens[player][i].x = (aliens[player][i].x +
						 (LAND_WIDTH * 32));
			}
		      else if (aliens[player][i].x > LAND_WIDTH * 32)
			{
			  aliens[player][i].x = (aliens[player][i].x -
						 (LAND_WIDTH * 32));
			}
		      
		      
		      /* Move vertically: */
		      
		      aliens[player][i].y = (aliens[player][i].y +
					     aliens[player][i].ym);
		      
		      if (land[player][aliens[player][i].x / 32]
			  [(aliens[player][i].y / 32) + 1]
			  != -1)
			{
			  aliens[player][i].y = aliens[player][i].y - 2;
			  aliens[player][i].ym = -2;
			  aliens[player][i].timer = (rand() % 20) + 20;
			}
		      
		      if (aliens[player][i].y < 80)
			{
			  aliens[player][i].y = 80;
			  aliens[player][i].ym = 2;
			  aliens[player][i].timer = (rand() % 20) + 20;
			}

		      if (aliens[player][i].y >= 448)
			{
			  aliens[player][i].y = 448;
			  aliens[player][i].ym = 2;
			}
		      
		      
		      /* Change direction? */
		      
		      aliens[player][i].timer--;
		      
		      if (aliens[player][i].timer <= 0)
			{
			  aliens[player][i].ym = 2;
			  aliens[player][i].timer = (rand() % 20) + 20;
			}
		      
		      
		      /* Home-in on a helpless penguinoid!? */
		      
		      if ((rand() %
			   (CHANCE_UFO_HOME_PENG / (level[player] + 1))) == 0)
			{
			  /* Are there any close by? */
			  
			  aliens[player][i].home_peng = -1;
			  
			  for (j = 0; (j < MAX_PENGUINOIDS &&
				       aliens[player][i].home_peng == -1); j++)
			    {
			      if (penguinoids[player][j].alive &&
				  (penguinoids[player][j].x -
				   aliens[player][i].x) > -200 &&
				  (penguinoids[player][j].x -
				   aliens[player][i].x) < 200 &&
				  penguinoids[player][j].being_homed == 0)
				{
				  aliens[player][i].mode = ALIEN_MODE_HOME_PENG;
				  aliens[player][i].home_peng = j;
				  aliens[player][i].beam_height = 0;
				  penguinoids[player][j].being_homed = 1;
				}
			    }
			}
		    }
		  else if (aliens[player][i].mode == ALIEN_MODE_HOME_PENG ||
			   aliens[player][i].mode == ALIEN_MODE_BEAMING_PENG)
		    {
		      /* Homing in-on, or beaming-up a penguinoid: */
		      
		      /* Move horizontally: */
		      
		      aliens[player][i].x = (aliens[player][i].x +
					     aliens[player][i].xm);
		      
		      if (aliens[player][i].x < 0)
			{
			  aliens[player][i].x = (aliens[player][i].x +
						 (LAND_WIDTH * 32));
			}
		      else if (aliens[player][i].x > LAND_WIDTH * 32)
			{
			  aliens[player][i].x = (aliens[player][i].x -
						 (LAND_WIDTH * 32));
			}
		      
		      
		      /* Home-in on, and beam-up the penguinoid! */
		      
		      if (aliens[player][i].x >
			  penguinoids[player]
			  [aliens[player][i].home_peng].x + 2)
			{
			  aliens[player][i].xm = -4;
			}
		      else if (aliens[player][i].x <
			       penguinoids[player]
			       [aliens[player][i].home_peng].x - 2)
			{
			  aliens[player][i].xm = 4;
			}
		      else
			{
			  /* Beam-up! */
			  
			  aliens[player][i].xm = 0;
			  aliens[player][i].x =
			    penguinoids[player]
			    [aliens[player][i].home_peng].x;
			  
			  if (aliens[player][i].y >=
			      penguinoids[player]
			      [aliens[player][i].home_peng].y - 34 &&
			      aliens[player][i].mode !=
			      ALIEN_MODE_BEAMING_PENG)
			    {
			      aliens[player][i].mode =
				ALIEN_MODE_BEAMING_PENG;
			      
			      penguinoids[player]
				[aliens[player][i].home_peng].mode =
				PENG_MODE_BEAMING;
			      
			      playsound(SND_PENG_CAPTURE, 2, 1);
			      set_message(IMG_PENGUINOID_IN_TROUBLE);
			    }
			}
		      
		      
		      /* Move vertically: */
		      
		      aliens[player][i].y = aliens[player][i].y + 4;
		      
		      if (aliens[player][i].y >
			  penguinoids[player]
			  [aliens[player][i].home_peng].y - 32)
			{
			  aliens[player][i].y =
			    penguinoids[player][aliens[player][i].home_peng].y -
			    32;
			}
		      
		      
		      /* Handle the beam-up graphic: */
		      
		      if (aliens[player][i].mode == ALIEN_MODE_BEAMING_PENG &&
			  (frame % 2) == 0)
			{
			  aliens[player][i].beam_height++;
			  
			  
			  /* Is it captured all the way!? */
			  
			  if (aliens[player][i].beam_height == 32)
			    {
			      aliens[player][i].mode = ALIEN_MODE_CAPTURED_PENG;
			      penguinoids[player]
				[aliens[player][i].home_peng].mode =
				PENG_MODE_CAPTURED;
			    }
			}
		      
		      
		      /* Is the penguinoid still alive!? */
		      
		      if (penguinoids[player]
			  [aliens[player][i].home_peng].alive == 0)
			{
			  aliens[player][i].mode = ALIEN_MODE_NORMAL;
			  aliens[player][i].xm = 4;
			  if ((rand() % 2) == 0)
			    aliens[player][i].xm = -aliens[player][i].xm;
			}
		    }
		  else if (aliens[player][i].mode == ALIEN_MODE_CAPTURED_PENG)
		    {
		      /* Captured a penguinoid!  Fly upwards! */
		      
		      aliens[player][i].y = aliens[player][i].y - 4;
		      
		      
		      /* Turn a penguinoid and a UFO into a mutant: */
		      
		      if (aliens[player][i].y < 80)
			{
			  aliens[player][i].y = 80;
			  aliens[player][i].type = ALIEN_MUTANT;
			  aliens[player][i].mode = ALIEN_MODE_HOME_SHIP;
			  
			  penguinoids[player]
			    [aliens[player][i].home_peng].alive = 0;
			  playsound(SND_PENG_MUTATE, 2, 0);
			  set_message(IMG_PENGUINOID_MUTATED);
			  
			  flash = 1;
			}
		      
		      penguinoids[player][aliens[player][i].home_peng].y =
			aliens[player][i].y + 32;
		      
		      
		      /* Is the penguinoid still alive!? */
		      
		      if (penguinoids[player]
			  [aliens[player][i].home_peng].alive == 0)
			{
			  aliens[player][i].mode = ALIEN_MODE_NORMAL;
			  aliens[player][i].xm = 4;
			  if ((rand() % 2) == 0)
			    aliens[player][i].xm = -aliens[player][i].xm;
			}
		    }


		  /* Shoot at the ship? */
		  
		  if ((aliens[player][i].mode == ALIEN_MODE_HOME_PENG &&
		       (rand() %
			((CHANCE_UFO_SHOOT_HOME /
			  ((level[player] / 2) + 1)) + 1))
		       == 0) ||
		      (aliens[player][i].mode == ALIEN_MODE_NORMAL &&
		       (rand() %
			((CHANCE_UFO_SHOOT_NORMAL /
			  ((level[player] / 2) + 1)) + 1))
		       == 0))
		    {
		      if (abs(x[player] - aliens[player][i].x) <= 320)
			{
			  add_bullet(aliens[player][i].x + 4,
				     aliens[player][i].y + 4, i);
			}
		    } 
		}
	      else if (aliens[player][i].type == ALIEN_BOMBER)
		{
		  /* Move horizontally: */
		  
		  aliens[player][i].x = (aliens[player][i].x +
					 aliens[player][i].xm);

		  if (aliens[player][i].x < 0)
		    {
		      aliens[player][i].x = (aliens[player][i].x +
					     (LAND_WIDTH * 32));
		    }
		  else if (aliens[player][i].x > LAND_WIDTH * 32)
		    {
		      aliens[player][i].x = (aliens[player][i].x -
					     (LAND_WIDTH * 32));
		    }
		  
		  
		  /* Move vertically: */
		  
		  aliens[player][i].y = (aliens[player][i].y +
					 aliens[player][i].ym);
		  
		  if (aliens[player][i].y < 80)
		    aliens[player][i].y = 448;
		  else if (aliens[player][i].y > 448)
		    aliens[player][i].y = 80;
		  
		  
		  /* Change direction: */
		  
		  if ((frame % 10) == 0)
		    {
		      aliens[player][i].ym = (aliens[player][i].ym +
					      aliens[player][i].ymm);
		      
		      if (aliens[player][i].ym < -4)
			aliens[player][i].ymm = 1;
		      else if (aliens[player][i].ym > 4)
			aliens[player][i].ymm = -1;
		    }
		  
		  
		  /* Count down timer: */
		  
		  aliens[player][i].timer--;
		  
		  if (aliens[player][i].timer < 0)
		    aliens[player][i].timer = 300;
		  
		  
		  /* Occassionally add a mine: */
		  
		  if (aliens[player][i].mode == ALIEN_MODE_NORMAL &&
		      aliens[player][i].timer < 99 &&
		      (aliens[player][i].timer % 33) == 0)
		    {
		      add_alien(aliens[player][i].x, aliens[player][i].y,
				ALIEN_MINE, ALIEN_MODE_NORMAL, 200);
		    }
		}
	      else if (aliens[player][i].type == ALIEN_POD)
		{
		  /* Move horizontally: */
		  
		  aliens[player][i].x = (aliens[player][i].x +
					 aliens[player][i].xm);

		  if (aliens[player][i].x < 0)
		    {
		      aliens[player][i].x = (aliens[player][i].x +
					     (LAND_WIDTH * 32));
		    }
		  else if (aliens[player][i].x > LAND_WIDTH * 32)
		    {
		      aliens[player][i].x = (aliens[player][i].x -
					     (LAND_WIDTH * 32));
		    }
		  
		  /* Move vertically: */
		  
		  aliens[player][i].y = (aliens[player][i].y +
					 aliens[player][i].ym);
		  
		  if (aliens[player][i].y < 80)
		    {
		      aliens[player][i].y = 80;
		      aliens[player][i].ym = (rand() % 4) + 1;
		    }
		  else if (aliens[player][i].y > 448)
		    {
		      aliens[player][i].y = 448;
		      aliens[player][i].ym = -((rand() % 4) + 1);
		    }
		  
		  
		  /* Count down timer: */
		  
		  aliens[player][i].timer--;
		  
		  
		  /* Move randomly: */
		  
		  if (aliens[player][i].timer < 0)
		    {
		      aliens[player][i].timer = (rand() % 200) + 100;
		      
		      aliens[player][i].xm = (rand() % 4) + 1;
		      if ((rand() % 2) == 0)
			aliens[player][i].xm = -aliens[player][i].xm;
		      
		      aliens[player][i].ym = (rand() % 4) + 1;
		      if ((rand() % 2) == 0)
			aliens[player][i].ym = -aliens[player][i].ym;
		    }
		}
	      else if (aliens[player][i].type == ALIEN_MINE)
		{
		  /* Count down timer: */
		  
		  aliens[player][i].timer--;
		  
		  
		  /* Die when our time's up: */
		  
		  if (aliens[player][i].timer < 0)
		    aliens[player][i].alive = 0;
		}
	      else if (aliens[player][i].type == ALIEN_MUTANT)
		{
		  if ((rand() % 10) > 3)
		    {
		      /* Home in on ship! */
		      
		      if (aliens[player][i].x < x[player])
			{
			  aliens[player][i].x = (aliens[player][i].x +
						 (16 - (rand() % 8)));
			}
		      else if (aliens[player][i].x >= x[player])
			{
			  aliens[player][i].x = (aliens[player][i].x -
						 (16 - (rand() % 8)));
			}
		      
		      /* if (aliens[player][i].y < y[player])
			 aliens[player][i].y = aliens[player][i].y +
			 (4 - (rand() % 2));
			 else if (aliens[player][i].y >= y[player])
			 aliens[player][i].y = aliens[player][i].y -
			 (4 - (rand() % 2));
		      */
		      
		      aliens[player][i].y = (aliens[player][i].y +
				     (my_cos[((frame + i) % 32)]) * 8);
		    }
		  else
		    {
		      /* Jitter: */
		      
		      aliens[player][i].x = (aliens[player][i].x +
					     ((rand() % 16) - 8));

		      aliens[player][i].y = (aliens[player][i].y +
					     ((rand() % 5) - 2));
		    }


		  /* Spin around: */
		  
		  /* aliens[player][i].x = (aliens[player][i].x +
		     my_cos[(aliens[player][i].timer % 32)] * 5);
		     aliens[player][i].y = (aliens[player][i].y -
		     my_cos[32 - (aliens[player][i].timer % 32)] * 5); */
		  
		  
		  /* Keep in-bounds: */
		  
		  if (aliens[player][i].x < 0)
		    {
		      aliens[player][i].x = (aliens[player][i].x +
					     (LAND_WIDTH * 32));
		    }
		  else if (aliens[player][i].x > LAND_WIDTH * 32)
		    {
		      aliens[player][i].x = (aliens[player][i].x -
					     (LAND_WIDTH * 32));
		    }
		  
		  if (aliens[player][i].y < 80)
		    aliens[player][i].y = 80;
		  else if (aliens[player][i].y > 448)
		    aliens[player][i].y = 448;


		  /* Shoot at the ship? */
		  
		  if (aliens[player][i].mode == ALIEN_MODE_NORMAL &&
		      (rand() %
		       ((CHANCE_MUTANT_SHOOT /
			 ((level[player] / 2) + 1)) + 1)) == 0)
		    {
		      if (x[player] - aliens[player][i].x >= -320 &&
			  x[player] - aliens[player][i].x <= 320)
			{
			  add_bullet(aliens[player][i].x + 12,
				     aliens[player][i].y + 8, i);
			}
		    } 
		}
	      else if (aliens[player][i].type == ALIEN_SWARMER)
		{
		  /* Home in on ship! */
		  
		  if ((frame % 10) != 0 &&
		      aliens[player][i].x >= x[player] - 600 &&
		      aliens[player][i].x <= x[player] + 600)
		    {
		      if (aliens[player][i].x < x[player])
			aliens[player][i].x = aliens[player][i].x + 8;
		      else if (aliens[player][i].x > x[player])
			aliens[player][i].x = aliens[player][i].x - 8;
		      
		      if (aliens[player][i].y < y[player])
			{
			  aliens[player][i].ym = aliens[player][i].ym + 1;
			  if (aliens[player][i].ym > 4)
			    aliens[player][i].ym = 4;
			}
		      else if (aliens[player][i].y > y[player])
			{
			  aliens[player][i].ym = aliens[player][i].ym - 1;
			  if (aliens[player][i].ym < -4)
			    aliens[player][i].ym = -4;
			}
		    }
		  else
		    {
		      aliens[player][i].x = (aliens[player][i].x +
					     aliens[player][i].xm);
		      
		      aliens[player][i].timer--;
		      if (aliens[player][i].timer <= 0)
			{
			  aliens[player][i].timer = (rand() % 50) + 50;
			  
			  aliens[player][i].xm = (rand() % 6) + 1;
			  if ((rand() % 2) == 0)
			    aliens[player][i].xm = -aliens[player][i].xm;
			  
			  aliens[player][i].ym = (rand() % 6) + 1;
			  if ((rand() % 2) == 0)
			    aliens[player][i].ym = -aliens[player][i].ym;
			}
		    }

		  aliens[player][i].y = (aliens[player][i].y +
					 aliens[player][i].ym);
		  
		  
		  /* Keep in-bounds: */
		  
		  if (aliens[player][i].x < 0)
		    {
		      aliens[player][i].x = (aliens[player][i].x +
					     (LAND_WIDTH * 32));
		    }
		  else if (aliens[player][i].x > LAND_WIDTH * 32)
		    {
		      aliens[player][i].x = (aliens[player][i].x -
					     (LAND_WIDTH * 32));
		    }
		  
		  if (aliens[player][i].y < 80)
		    aliens[player][i].y = 80;
		  else if (aliens[player][i].y > 448)
		    aliens[player][i].y = 448;
		  
		  
		  /* Shoot at the ship? */
		  
		  if ((rand() %
		       ((CHANCE_SWARMER_SHOOT /
			 ((level[player] / 2) + 1)) + 1))
		      == 0)
		    {
		      if (x[player] - aliens[player][i].x >= -320 &&
			  x[player] - aliens[player][i].x <= 320)
			{
			  add_bullet(aliens[player][i].x,
				     aliens[player][i].y, i);
			}
		    } 
		}
	      else if (aliens[player][i].type == ALIEN_BAITER)
		{
		  /* Move alien: */
		  
		  aliens[player][i].x = (aliens[player][i].x +
					 aliens[player][i].xm);
		  
		  aliens[player][i].y = (aliens[player][i].y +
					 aliens[player][i].ym);
		  
		  
		  /* Keep in-bounds: */
		  
		  if (aliens[player][i].x < 0)
		    {
		      aliens[player][i].x = (aliens[player][i].x +
					     (LAND_WIDTH * 32));
		    }
		  else if (aliens[player][i].x > LAND_WIDTH * 32)
		    {
		      aliens[player][i].x = (aliens[player][i].x -
					     (LAND_WIDTH * 32));
		    }
		  
		  if (aliens[player][i].y < 80)
		    {
		      aliens[player][i].y = 80;
		      aliens[player][i].ym = 0;
		    }
		  else if (aliens[player][i].y > 448)
		    {
		      aliens[player][i].y = 448;
		      aliens[player][i].ym = 0;
		    }
		  
		  
		  /* Home-in on player: */
		  
		  if (aliens[player][i].x > x[player])
		    {
		      aliens[player][i].xm--;
		      if (aliens[player][i].xm < -16)
			aliens[player][i].xm = -16;
		    }
		  else if (aliens[player][i].x < x[player])
		    {
		      aliens[player][i].xm++;
		      if (aliens[player][i].xm > 16)
			aliens[player][i].xm = 16;
		    }
		  
		  if (aliens[player][i].y > y[player])
		    {
		      aliens[player][i].ym--;
		      if (aliens[player][i].ym < -8)
			aliens[player][i].ym = -8;
		    }
		  else if (aliens[player][i].y < y[player])
		    {
		      aliens[player][i].ym++;
		      if (aliens[player][i].ym > 32)
			aliens[player][i].ym = 32;
		    }


		  /* Shoot at the ship? */
		  
		  if ((rand() %
		       ((CHANCE_BAITER_SHOOT /
			 ((level[player] / 2) + 1)) + 1)) == 0)
		    {
		      if (x[player] - aliens[player][i].x >= -320 &&
			  x[player] - aliens[player][i].x <= 320)
			{
			  add_bullet(aliens[player][i].x,
				     aliens[player][i].y, i);
			}
		    } 
		}
	      else if (aliens[player][i].type == ALIEN_EVILBILL)
		{
		  /* Move alien: */
		  
		  if (aliens[player][i].mode == ALIEN_MODE_NORMAL ||
		      (aliens[player][i].mode == ALIEN_MODE_HURTING &&
		       aliens[player][i].timer < 5))
		    {
		      aliens[player][i].x = (aliens[player][i].x +
					     aliens[player][i].xm);
		      
		      aliens[player][i].y = (aliens[player][i].y +
					     aliens[player][i].ym);
		      
		      
		      /* Keep in-bounds: */
		      
		      if (aliens[player][i].x < 0)
			{
			  aliens[player][i].x = (aliens[player][i].x +
						 (LAND_WIDTH * 32));
			}
		      else if (aliens[player][i].x > LAND_WIDTH * 32)
			{
			  aliens[player][i].x = (aliens[player][i].x -
						 (LAND_WIDTH * 32));
			}
		      
		      if (aliens[player][i].y < 80)
			{
			  aliens[player][i].y = 80;
			  aliens[player][i].ym = 0;
			}
		      else if (aliens[player][i].y > 448)
			{
			  aliens[player][i].y = 448;
			  aliens[player][i].ym = 0;
			}
		      
		      
		      /* Home-in on player: */
		      
		      if (aliens[player][i].x > x[player])
			{
			  aliens[player][i].xm--;
			  if (aliens[player][i].xm < -16)
			    aliens[player][i].xm = -16;
			}
		      else if (aliens[player][i].x < x[player])
			{
			  aliens[player][i].xm++;
			  if (aliens[player][i].xm > 16)
			    aliens[player][i].xm = 16;
			}
		      
		      if (aliens[player][i].y > y[player])
			{
			  aliens[player][i].ym--;
			  if (aliens[player][i].ym < -8)
			    aliens[player][i].ym = -8;
			}
		      else if (aliens[player][i].y < y[player])
			{
			  aliens[player][i].ym++;
			  if (aliens[player][i].ym > 32)
			    aliens[player][i].ym = 32;
			}
		    }
		  else if (aliens[player][i].mode == ALIEN_MODE_DYING)
		    {
		      /* Crash to the ground: */
		      
		      aliens[player][i].y = (aliens[player][i].y +
					     aliens[player][i].ym);
		      
		      if (frame % 4)
			aliens[player][i].ym++;
		      
		      if (aliens[player][i].y >= 480)
			{
			  kill_alien(i);
			  flash = 50;
			  flash_colors = 1;
			}


		      /* Spray out bits: */
		      
		      add_explosion(aliens[player][i].x + (rand() % 128),
				    aliens[player][i].y + (rand() % 128),
				    IMG_LASERS);
		    }
		  
		  
		  /* Shoot at the ship? */
		  
		  if ((aliens[player][i].mode == ALIEN_MODE_NORMAL ||
		       (aliens[player][i].mode == ALIEN_MODE_HURTING &&
			aliens[player][i].timer < 2)) &&
		      (rand() % CHANCE_EVILBILL_SHOOT) == 0)
		    {
		      if (x[player] - aliens[player][i].x >= -320 &&
			  x[player] - aliens[player][i].x <= 320)
			{
			  add_bullet(aliens[player][i].x + 64,
				     aliens[player][i].y + 64, i);
			  aliens[player][i].mode = ALIEN_MODE_SHOOTING;
			  aliens[player][i].timer = 5;
			  playsound(SND_EVILBILL_SHOOT, 1, 0);
			}
		    } 
		  
		  
		  /* Drop a mine!? */
		  
		  if (aliens[player][i].mode == ALIEN_MODE_NORMAL &&
		      (rand() % CHANCE_EVILBILL_MINE) == 0)
		    {
		      add_alien(aliens[player][i].x + 48,
				aliens[player][i].y + 48, ALIEN_MINE,
				ALIEN_MODE_NORMAL, 200);
		      playsound(SND_EVILBILL_MINE, 1, 0);
		    }
		  
		  
		  /* Make flames if hurt a lot: */
		  
		  if (aliens[player][i].shields < 8 && (rand() % 5) == 0)
		    {
		      add_flame(aliens[player][i].x + (rand() % 160) - 16,
				aliens[player][i].y + (rand() % 160) - 16);
		    }
		  
		  
		  /* Count down timer: */
		  
		  if (aliens[player][i].timer > 0)
		    {
		      aliens[player][i].timer--;
		      
		      if (aliens[player][i].timer == 0 &&
			  aliens[player][i].mode != ALIEN_MODE_DYING)
			{
			  aliens[player][i].mode = ALIEN_MODE_NORMAL;
			}
		    }
		}
	      
	      
	      /* Handle beaming-in of all aliens: */
	      
	      if (aliens[player][i].mode == ALIEN_MODE_BEAM_IN)
		{
		  aliens[player][i].timer--;
		  
		  if (aliens[player][i].timer <= 0)
		    {
		      aliens[player][i].mode = ALIEN_MODE_NORMAL;
		      aliens[player][i].timer = (rand() % 20) + 20;
		    }
		}
	      
	      
	      if (aliens[player][i].type != ALIEN_MINE)
		{
		  /* Kill aliens if shot by our laser: */
		  
		  /* (determine relative position on screen (laser
		     is screenbased)) */
		  
		  tmp_x = aliens[player][i].x - (x[player] - scroll[player]);
		  
		  if (tmp_x < -31)
		    tmp_x = tmp_x + (LAND_WIDTH * 32);
		  
		  while (tmp_x >= (LAND_WIDTH * 32))
		    tmp_x = tmp_x - (LAND_WIDTH * 32);
		  
		  
		  /* (see if any lasers are touching the alien) */
		  
		  for (j = 0; j < MAX_LASERS; j++)
		    {
		      if (lasers[j].alive && aliens[player][i].alive)
			{
			  if (((lasers[j].x2 == 640 &&
				tmp_x >= lasers[j].x1 && tmp_x <= 640) ||
			       (lasers[j].x2 == 0 &&
				tmp_x <= lasers[j].x1 - 32 && tmp_x >= 0)) &&
			      (aliens[player][i].y + ysize >=
			       lasers[j].y - 32 &&
			       aliens[player][i].y <= lasers[j].y) &&
			      aliens[player][i].mode != ALIEN_MODE_HURTING &&
			      aliens[player][i].mode != ALIEN_MODE_DYING)
			    {
			      /* Kill the alien: */
			      
			      kill_alien(i);
			      
			      
			      /* If it was a pod, make a bunch of swarmers: */
			      
			      if (aliens[player][i].type == ALIEN_POD)
				{
				  add_alien(aliens[player][i].x - 32,
					    aliens[player][i].y - 64,
					    ALIEN_SWARMER,
					    ALIEN_MODE_NORMAL, 1);	

				  add_alien(aliens[player][i].x + 32,
					    aliens[player][i].y - 64,
					    ALIEN_SWARMER,
					    ALIEN_MODE_NORMAL, 1);

				  add_alien(aliens[player][i].x - 32,
					    aliens[player][i].y + 64,
					    ALIEN_SWARMER,
					    ALIEN_MODE_NORMAL, 1);

				  add_alien(aliens[player][i].x + 32,
					    aliens[player][i].y + 64,
					    ALIEN_SWARMER,
					    ALIEN_MODE_NORMAL, 1);
				  
				  playsound(SND_SWARMERS, 1, 0);
				}
			    }
			}
		    }
		}

	      
	      /* See if we've collided with the player! */
	      
	      if (safe[player] == 0 && dying[player] == 0 &&
		  dancing[player] == 0 &&
		  aliens[player][i].x + xsize >= (x[player] +
						  (xm[player] * 8) - 16) &&
		  aliens[player][i].x <= x[player] + (xm[player] * 8) + 80 &&
		  aliens[player][i].y + ysize >= y[player] - 16 &&
		  aliens[player][i].y <= y[player] + 16 &&
		  aliens[player][i].mode != ALIEN_MODE_BEAM_IN &&
		  aliens[player][i].mode != ALIEN_MODE_HURTING &&
		  aliens[player][i].mode != ALIEN_MODE_DYING)
		{
		  /* Kill the alien: */
		  
		  kill_alien(i);
		  
		  
		  /* Kill the player: */
		  
		  kill_player();
		}
	    }
	}
      
      
      /* Beam more aliens in if they're all dead early... */
      
      if (num_aliens == 0 &&
	  level_time[player] > 100 && level_time[player] < 1000)
	{
	  level_time[player] = 999;
	}
      
      
      /* Advance to the next level? */
      
      if (num_aliens == 0 && level_time[player] > 1000 &&
	  level_beat[player] == 0 && level[player] != 20)
	{
	  level_beat[player] = 1;
	  level[player]++;
	  
	  
	  /* Regenerate the planet every 5th level: */
	  
	  if (planet_dead[player] == 1 && (level[player] % 5) == 0)
	    {
	      planet_dead[player] = 0;
	      create_land(player);
	      create_penguinoids(player, MAX_PENGUINOIDS);
	    }
	  
	  
	  /* Get ready for last level music: */
	  
	  if (level[player] == 20)
	    {
//#ifdef SOUND
	      if (use_sound == 1)
		{
		  Mix_HaltMusic();
		}
//#endif /* #ifdef SOUND */
	    }
	}
      
      
      /* Has the level been beat? */
      
      if (level_beat[player] > 0)
	{
	  level_beat[player]++;
	  
	  
	  if (level_beat[player] >= 170 - MAX_PENGUINOIDS &&
	      (level_beat[player] - (170 - MAX_PENGUINOIDS)) < num_penguinoids)
	    {
	      /* Explode bonus penguins: */
	      
	      add_points((((640 - (num_penguinoids * 32)) / 2) +
			  ((level_beat[player] -
			    (170 - MAX_PENGUINOIDS)) * 32) + 16 +
			  x[player] - scroll[player]),
			 80 + (images[IMG_LEVEL_BONUS] -> h) + 32,
			 IMG_100);
	      add_score(100);
	    }
	  
	  
	  if (level_beat[player] >= 170)
	    {
	      /* Reset clocks: */
	      
	      level_time[player] = 0;
	      level_beat[player] = 0;
	    }
	}
      
      
      /* Handle safe mode: */
      
      if (safe[player] > 0)
	safe[player]--;
      
      
      /* Handle scrolling position: */
      
      if (scroll[player] > 128 && dir[player] == DIR_RIGHT)
	scroll[player] = scroll[player] - 16;
      else if (scroll[player] < 448 && dir[player] == DIR_LEFT)
	scroll[player] = scroll[player] + 16;
      
      
      /* Draw screen: */
      
      /* (Erase to background): */
      
      dest.x = 0;
      dest.y = 0;
      dest.w = 320;
      dest.h = 240;
      
      if (flash == 0 || (frame % 2) == 0)
	SDL_FillRect(screen, &dest,
		     SDL_MapRGB(screen->format, 0x00, 0x00, 0x00));
      else
	{
	  if (flash_colors == 0)
	    {
	      SDL_FillRect(screen, &dest,
			   SDL_MapRGB(screen->format, 0xFF, 0xFF, 0xFF));
	    }
	  else
	    {
	      SDL_FillRect(screen, &dest,
			   SDL_MapRGB(screen->format,
				      (Uint8) (rand() % 256),
				      (Uint8) (rand() % 256),
				      (Uint8) (rand() % 256)));
	    }
	  flash--;
	  
	  if (flash <= 0)
	    flash_colors = 0;
	}
      
      
      /* (status area): */
      
      dest.x = 0;
      dest.y = 0;
      dest.w = 320;
      dest.h = 40;
      
      SDL_BlitSurface(images[IMG_STATUS_AREA], NULL, screen, &dest);
      
      
      /* (radar details): */
      
      /* ((ground)): */
      
      for (i = 0; i < LAND_WIDTH; i++)
	{
	  for (j = 0; j < LAND_HEIGHT; j++)
	    {
	      if (land[player][i][j] != -1)
		{
		  dest.x = ((i * 32) - (x[player] - scroll[player])
			    + 3040);
		  
		  if (dest.x < 0)
		    dest.x = dest.x + (LAND_WIDTH * 32);
		  
		  while (dest.x >= (LAND_WIDTH * 32))
		    dest.x = dest.x - (LAND_WIDTH * 32);
		  
		  dest.x = (dest.x / 16) + 88;
		  dest.y = (j * 2) + 3;
		  dest.w = 2;
		  dest.h = 2;
		  
		  if (land[player][i][j] == IMG_LAND_LEFT)
		    src.x = 0;
		  else if (land[player][i][j] == IMG_LAND_CENTER)
		    src.x = 2;
		  else if (land[player][i][j] == IMG_LAND_RIGHT)
		    src.x = 4;
		  
		  src.y = 0;
		  src.w = 2;
		  src.h = 2;
		  
		  SDL_BlitSurface(images[IMG_MAP_LAND], &src, screen, &dest);
		}
	    }
	}
      
      
      /* ((penguinoids)): */
      
      for (i = 0; i < MAX_PENGUINOIDS; i++)
	{
	  if (penguinoids[player][i].alive)
	    {
	      dest.x = (penguinoids[player][i].x - (x[player] - scroll[player])
			+ 3040);
	      
	      if (dest.x < 0)
		dest.x = dest.x + (LAND_WIDTH * 32);
	      
	      while (dest.x >= (LAND_WIDTH * 32))
		dest.x = dest.x - (LAND_WIDTH * 32);
	      
	      dest.x = (dest.x / 16) + 88;
	      dest.y = (penguinoids[player][i].y / 16) + 3;
	      dest.w = 2;
	      dest.h = 2;
	      
	      SDL_BlitSurface(images[IMG_MAP_PENG], NULL, screen, &dest);
	      
	      
	      /* Draw an alert if he's captured! */
	      
	      if ((penguinoids[player][i].mode == PENG_MODE_BEAMING ||
		   penguinoids[player][i].mode == PENG_MODE_CAPTURED) &&
		  (frame % 2) == 0)
		{
		  dest.x = dest.x - 1;
		  dest.y = dest.y - 1;
		  dest.w = 4;
		  dest.h = 4;
		  
		  SDL_BlitSurface(images[IMG_MAP_ALERT], NULL, screen, &dest);
		}
	    }
	}
      
      
      /* ((aliens)): */
      
      for (i = 0; i < MAX_ALIENS; i++)
	{
	  if (aliens[player][i].alive &&
	      (aliens[player][i].mode != ALIEN_MODE_BEAM_IN ||
	       (frame % 2) == 0))
	    {
	      /* Determine it's position on the radar: */
	      
	      dest.x = (aliens[player][i].x - (x[player] - scroll[player])
			+ 3040);
	      
	      if (dest.x < 0)
		dest.x = dest.x + (LAND_WIDTH * 32);
	      
	      while (dest.x >= (LAND_WIDTH * 32))
		dest.x = dest.x - (LAND_WIDTH * 32);
	      
	      dest.x = (dest.x / 16) + 88;
	      dest.y = (aliens[player][i].y / 16) + 3;
	      dest.w = 2;
	      dest.h = 2;
	      
	      
	      /* Which picture to use: */
	      
	      if (aliens[player][i].type == ALIEN_UFO)
		img = IMG_MAP_UFO;
	      else if (aliens[player][i].type == ALIEN_BOMBER)
		img = IMG_MAP_BOMBER;
	      else if (aliens[player][i].type == ALIEN_MINE)
		img = IMG_MAP_MINE;
	      else if (aliens[player][i].type == ALIEN_POD)
		img = IMG_MAP_POD;
	      else if (aliens[player][i].type == ALIEN_SWARMER)
		img = IMG_MAP_SWARMER;
	      else if (aliens[player][i].type == ALIEN_MUTANT)
		img = IMG_MAP_MUTANT;
	      else if (aliens[player][i].type == ALIEN_BAITER)
		img = IMG_MAP_BAITER;
	      else if (aliens[player][i].type == ALIEN_EVILBILL)
		img = IMG_MAP_EVILBILL;
	      
	      
	      /* Draw it! */
	      
	      SDL_BlitSurface(images[img], NULL, screen, &dest);
	    }
	}
      
      
      /* ((ship)): */
      
      dest.x = ((xm[player] * 8 + scroll[player]) / 16) + 140;
      dest.y = (y[player] / 16) + 3;
      dest.w = 3;
      dest.h = 2;
      
      SDL_BlitSurface(images[IMG_MAP_SHIP], NULL, screen, &dest);
      
      
      /* (each player's status): */
      
      for (i = 0; i < num_players; i++)
	{
	  /* ((number of ships)): */
	  
	  for (j = 0; j < lives[i] && j < 11; j++)
	    {
	      if (lives[i] < 7)
		dest.x = j * 12 + (235 * i);
	      else
		dest.x = j * 8 + (235 * i);
	      
	      dest.y = 12;
	      dest.w = 12;
	      dest.h = 8;
	      
	      SDL_BlitSurface(images[IMG_SHIPS], NULL, screen, &dest);
	    }
	  
	  
	  /* ((number of bombs)): */
	  
	  for (j = 0; j < bombs[i] && j < 9; j++)
	    {
	      if (i == 0)
		dest.x = 72;
	      else
		dest.x = 308;
	      
	      if (bombs[i] < 6)
		dest.y = 30 - (j * 6);
	      else
		dest.y = 30 - (j * 3);
	      
	      dest.w = 12;
	      dest.h = 6;
	      
	      SDL_BlitSurface(images[IMG_SMART_BOMB], NULL, screen, &dest);
	    }
	  
	  
	  /* ((score)): */
	  
	  if (player != i || (frame % 10) < 5)
	    draw_number(235 * i, 20, score[i]);
	  
	  
	  /* ((level)): */
	  draw_number(235 * i, 0, level[i]);
	}


      /* (stars): */
      
      for (i = 0; i < MAX_STARS; i++)
	{
	  /* Width of stars depends on speed of ship: */
	  
	  j = abs(xm[player] / 2);
	  if (j == 0)
	    j = 1;
	  
	  
	  /* Horizontal position of star? */
	  
	  dest.x = stars[i].x - (x[player] - scroll[player]) / 2 - j;
	  
	  if (dest.x < -31)
	    dest.x = dest.x + (LAND_WIDTH * 16);
	  
	  while (dest.x >= (LAND_WIDTH * 16))
	    dest.x = dest.x - (LAND_WIDTH * 16);
	  
	  if (dest.x > -31 && dest.x < 640)
	    {
	      dest.x /= 2;
	      dest.y = stars[i].y /2;
	      dest.w = j;
	      dest.h = 1;
	      
	      src.x = 0;
	      src.y = (rand() % 16);
	      src.w = j;
	      src.h = 1;
	      
	      SDL_BlitSurface(images[IMG_LASERS], &src, screen, &dest);
	    }
	}
     
      
      /* (ground): */
      
      for (j = 0; j < LAND_HEIGHT; j++)
	{
	  for (i = -1; i < 21; i++)
	    {
	      zz = i + ((x[player] - scroll[player]) / 32);
	      if (zz < 0)
		zz = zz + LAND_WIDTH;
	      else if (zz >= LAND_WIDTH)
		zz = zz - LAND_WIDTH;
	      
	      if (land[player][zz][j] != -1)
		{
		  dest.x = (i * 32) - ((x[player] - scroll[player]) % 32);
		  
		  if (dest.x > -31 && dest.x < 640)
		    {
		      dest.x /= 2;
		      dest.y = j * 16;
		      dest.w = 16;
		      dest.h = 16;
		      
		      SDL_BlitSurface(images[land[player][zz][j]],
				      NULL, screen, &dest);
		    }
		}
	    }
	}
      

      /* (explosion bits): */
      
      for (i = 0; i < MAX_EXPLOSION_BITS; i++)
	{
	  if (explosion_bits[i].alive)
	    {
	      dest.x = explosion_bits[i].x - (x[player] - scroll[player]);
	      
	      if (dest.x < -31)
		dest.x = dest.x + (LAND_WIDTH * 16);
	      
	      while (dest.x >= (LAND_WIDTH * 16))
		dest.x = dest.x - (LAND_WIDTH * 16);
	      
	      if (dest.x > -31 && dest.x < 640)
		{
		  dest.x /= 2;
		  dest.y = explosion_bits[i].y /2;
		  dest.w = 2;
		  dest.h = 2;
		  
		  src.x = (rand() % 13);
		  src.y = (rand() % 13);
		  src.w = 2;
		  src.h = 2;
		  
		  SDL_BlitSurface(images[explosion_bits[i].img],
				  &src, screen, &dest);
		}
	    }
	}
      
      
      /* (penguinoids): */
      
      for (i = 0; i < MAX_PENGUINOIDS; i++)
	{
	  if (penguinoids[player][i].alive)
	    {
	      dest.x = penguinoids[player][i].x - (x[player] - scroll[player]);
	      
	      if (dest.x < -31)
		dest.x = dest.x + (LAND_WIDTH * 32);
	      
	      while (dest.x >= (LAND_WIDTH * 32))
		dest.x = dest.x - (LAND_WIDTH * 32);
	      
	      if (dest.x > -31 && dest.x < 640)
		{
		  dest.x /= 2;
		  dest.y = penguinoids[player][i].y / 2;
		  dest.w = 16;
		  dest.h = 16;
		  
		  
		  if (level[player] != 20)
		    {
		      if (penguinoids[player][i].mode == PENG_MODE_SAVED)
			{
			  if (dir[player] == DIR_LEFT)
			    j = IMG_TUX_L0;
			  else
			    j = IMG_TUX_R0;
			}
		      else if (penguinoids[player][i].mode ==
			       PENG_MODE_FALLING ||
			       penguinoids[player][i].mode ==
			       PENG_MODE_CAPTURED)
			{
			  j = IMG_TUX_FALL0 + ((frame / 4) % 2);
			}
		      else
			{
			  if (penguinoids[player][i].xm < 0)
			    j = IMG_TUX_L0;
			  else
			    j = IMG_TUX_R0;
			  
			  j = j + (((frame + i) / 4) % 8);
			}
		    }
		  else
		    {
		      /* Final level!: */
		      
		      if (penguinoids[player][i].xm < 0)
			j = IMG_TUX_LFLAG;
		      else
			j = IMG_TUX_RFLAG;
		    }
		  
		  SDL_BlitSurface(images[j], NULL, screen, &dest);
		}
	    }
	}
      
      
      /* (aliens): */
      
      for (i = 0; i < MAX_ALIENS; i++)
	{
	  if (aliens[player][i].alive)
	    {
	      /* Determine it's horizontal position: */
	      
	      tmp_x = aliens[player][i].x - (x[player] - scroll[player]);
	      
	      if (tmp_x < -31)
		tmp_x = tmp_x + (LAND_WIDTH * 32);
		
	      while (tmp_x >= (LAND_WIDTH * 32))
		tmp_x = tmp_x - (LAND_WIDTH * 32);
	      
	      
	      if (tmp_x > -31 && tmp_x < 640)
		{
		  /* Which picture? */
		  
		  if (aliens[player][i].type == ALIEN_UFO)
		    img = IMG_UFO0 + ((frame + i) % 2);
		  else if (aliens[player][i].type == ALIEN_MUTANT)
		    img = IMG_MUTANT0 + ((frame / 4) % 2);
		  else if (aliens[player][i].type == ALIEN_BOMBER)
		    {
		      img = IMG_BOMBER;
		      
		      
		      /* (Is the bomber bulging?) */
		      
		      if (aliens[player][i].mode == ALIEN_MODE_NORMAL &&
			  aliens[player][i].timer < 100)
			{
			  if ((aliens[player][i].timer % 33) < 5)
			    img = IMG_BOMBER_BULGE1;
			  else if ((aliens[player][i].timer % 33) < 10)
			    img = IMG_BOMBER_BULGE0;
			}
		    }
		  else if (aliens[player][i].type == ALIEN_MINE)
		    {
		      if (aliens[player][i].timer > 3)
			{
			  if ((frame % 4) < 2 &&
			      abs(aliens[player][i].x - x[player]) < 100 &&
			      abs(aliens[player][i].y - y[player]) < 100)
			    img = IMG_MINE_FLASH;
			  else
			    img = IMG_MINE;
			}
		      else
			img = IMG_MINE_POP;
		    }
		  else if (aliens[player][i].type == ALIEN_POD)
		    {
		      img = IMG_POD0 + ((frame / 4) % 3);
		    }
		  else if (aliens[player][i].type == ALIEN_SWARMER)
		    {
		      img = IMG_SWARMER;
		    }
		  else if (aliens[player][i].type == ALIEN_BAITER)
		    {
		      img = IMG_BAITER0 + (frame % 5);
		    }
		  else if (aliens[player][i].type == ALIEN_EVILBILL)
		    {
		      if (aliens[player][i].mode == ALIEN_MODE_SHOOTING)
			img = IMG_EVILBILL_SHOOT;
		      else if (aliens[player][i].mode == ALIEN_MODE_HURTING ||
			       aliens[player][i].mode == ALIEN_MODE_DYING)
			img = IMG_EVILBILL_HURT1 + (frame % 2);
		      else
			img = IMG_EVILBILL1 + ((frame / 4) % 2);
		    }
		  
		  
		  /* Draw the damned thing already! */
		  
		  if (aliens[player][i].mode != ALIEN_MODE_BEAM_IN)
		    {
		      /* Not beaming in, just draw it: */
		      
		      dest.x = tmp_x /2;
		      dest.y = aliens[player][i].y /2;
		      dest.w = images[img] -> w;
		      dest.h = images[img] -> h;
		      
		      SDL_BlitSurface(images[img], NULL, screen, &dest);
		      
		      
		      /* Beaming-up a penguinoid!? */
		      
		      if (aliens[player][i].mode == ALIEN_MODE_BEAMING_PENG)
			{
			  dest.x = tmp_x/2;
			  dest.y = aliens[player][i].y/2 + 16;
			  dest.w = 16;
			  dest.h = aliens[player][i].beam_height/2;
			  
			  src.x = 0;
			  src.y = 0;
			  src.w = 16;
			  src.h = aliens[player][i].beam_height/2;
			  
			  SDL_BlitSurface(images[IMG_BEAM0 + (frame % 4)],
					  &src, screen, &dest);
			}
		    }
		  else
		    {
		      /* Beaming in - draw it beaming in! */
		      
		      for (j = (frame % 2); j < 32; j = j + 2)
			{
			  dest.x = (tmp_x +
				    (my_cos[(j + aliens[player][i].timer) % 32]
				     * (aliens[player][i].timer / 2)))/2;
			  dest.y = (aliens[player][i].y + j)/2;
			  dest.w = 16;
			  dest.h = 1;
			  
			  src.x = 0;
			  src.y = j/2;
			  src.w = 16;
			  src.h = 1;
			  
			  SDL_BlitSurface(images[img], &src, screen, &dest);
			}
		    }
		}
	    }
	}
      
      
      /* (ship): */
      
      dest.x = (xm[player] * 4) + scroll[player]/2;
      dest.y = y[player]/2;
      dest.w = 32;
      dest.h = 16;
      
      if (dancing[player] == 0)
	{
	  if (dying[player] == 0)
	    {
	      /* Ship: */
	      
	      if (safe[player] == 0 || (frame % 2) == 0)
		{
		  SDL_BlitSurface(images[IMG_SHIP_LEFT + (player * 2) +
					dir[player]],
				  NULL, screen, &dest);
		}
	    }
	  else if (dying[player] < 20)
	    {
	      /* Exploding ship: */
	      
	      SDL_BlitSurface(images[IMG_SHIP_LEFT + dir[player] + 4],
			      NULL, screen, &dest);
	    }
	}
      else
	{
	  /* Dancing Torvalds! */
	  
	  dest.h = 32;
	  
	  if (dancing[player] < 20)
	    {
	      /* Door is open... */
	      
	      SDL_BlitSurface(images[IMG_SHIP_WIN_0],
			      NULL, screen, &dest);
	    }
	  else
	    {
	      /* ... and there's Linus! */
	      
	      SDL_BlitSurface(images[IMG_SHIP_WIN_1 + ((frame / 10) % 2)],
			      NULL, screen, &dest);
	      
	      
	      if (dancing[player] > 200)
		{
		  /* Disco ball: */
		  
		  dest.x = 152;
		  dest.y = 40;
		  dest.w = 16;
		  dest.h = dancing[player] - 200;
		  
		  if (dest.h > (images[IMG_DISCO_1] -> h))
		    dest.h = (images[IMG_DISCO_1] -> h);

		  dest.h /= 2;
		  
		  src.x = 0;
		  src.y = images[IMG_DISCO_1] -> h - dest.h;
		  src.w = 16;
		  src.h = dest.h;
		  
		  SDL_BlitSurface(images[IMG_DISCO_1 + ((frame / 5) % 2)],
				  &src, screen, &dest);
		}
	    }
	}
      
      
      /* (oneup effect) */
      
      if (oneup_effect > 0)
	{
	  for (tmp_y = 0; tmp_y < 32; tmp_y = tmp_y + 4)
	    {
	      for (tmp_x = 0; tmp_x < 64; tmp_x = tmp_x + 4)
		{
		  dest.x = (xm[player] * 8) + scroll[player] +
		    ((tmp_x - 32) * oneup_effect) + 32;
		  dest.y = y[player] +
		    ((tmp_y - 16) * oneup_effect) + 16;
		  dest.x /= 2;
		  dest.y /= 2;
		  dest.w = 2;
		  dest.h = 2;
		  
		  src.x = tmp_x/2;
		  src.y = tmp_y/2;
		  src.w = 2;
		  src.h = 2;
		  
		  SDL_BlitSurface(images[IMG_SHIP_LEFT + (player * 2) +
					dir[player]],
				  &src, screen, &dest);
		}
	    }
	}
      
      
      /* (bullets): */
      
      for (i = 0; i < MAX_BULLETS; i++)
	{
	  if (bullets[i].alive)
	    {
	      tmp_x = bullets[i].x - (x[player] - scroll[player]);
	      
	      if (tmp_x < -31)
		tmp_x = tmp_x + (LAND_WIDTH * 32);
		
	      while (tmp_x >= (LAND_WIDTH * 32))
		tmp_x = tmp_x - (LAND_WIDTH * 32);
	      
	      if (tmp_x > -31 && tmp_x < 640)
		{
		  dest.x = tmp_x/2;
		  dest.y = bullets[i].y/2;
		  dest.w = 4;
		  dest.h = 4;
		  
		  SDL_BlitSurface(images[IMG_BULLET0 + ((i + frame) % 5)],
				  NULL, screen, &dest);
		}
	    }
	}
      
      
      /* (lasers): */
      
      for (i = 0; i < MAX_LASERS; i++)
	{
	  if (lasers[i].alive)
	    {
	      src.x = 0;
	      src.y = (rand() % 16);
	      src.w = 4;
	      src.h = 1;
	      
	      if (lasers[i].x1 < lasers[i].x2)
		{
		  x1 = lasers[i].x1;
		  x2 = lasers[i].x2;
		}
	      else
		{
		  x1 = lasers[i].x2;
		  x2 = lasers[i].x1;
		}
	      
	      for (tmp_x = x1; tmp_x < x2; tmp_x = tmp_x + 8)
		{
		  dest.x = tmp_x/2;
		  dest.y = lasers[i].y/2;
		  dest.w = 4;
		  dest.h = 1;
		  
		  SDL_BlitSurface(images[IMG_LASERS], &src, screen, &dest);
		}
	    }
	}
      
      
      /* (engine flames): */
      
      if ((right_down == 1 || thrust_down == 1) && dir[player] == DIR_RIGHT)
	{
	  dest.x = (xm[player] * 8) + scroll[player] - 32;
	  dest.x /= 2;
	  dest.y = y[player]/2;
	  dest.w = 16;
	  dest.h = 16;
	  
	  SDL_BlitSurface(images[IMG_FLAME_RIGHT_0 + (frame % 2)],
			  NULL, screen, &dest);
	}

      if ((left_down == 1 || thrust_down == 1) && dir[player] == DIR_LEFT)
	{
	  dest.x = (xm[player] * 8) + scroll[player] + 64;
	  dest.x /= 2;
	  dest.y = y[player]/2;
	  dest.w = 16;
	  dest.h = 16;
	  
	  SDL_BlitSurface(images[IMG_FLAME_LEFT_0 + (frame % 2)],
			  NULL, screen, &dest);
	}

      
      /* (evilbill-hurt flames): */
      
      for (i = 0; i < MAX_FLAMES; i++)
	{
	  if (flames[i].alive)
	    {
	      tmp_x = flames[i].x - (x[player] - scroll[player]);
	      
	      if (tmp_x < -31)
		tmp_x = tmp_x + (LAND_WIDTH * 32);
		
	      while (tmp_x >= (LAND_WIDTH * 32))
		tmp_x = tmp_x - (LAND_WIDTH * 32);
	      
	      if (tmp_x > -31 && tmp_x < 640)
		{
		  dest.x = tmp_x/2;
		  dest.y = flames[i].y/2;
		  dest.w = 16;
		  dest.h = 16;
		  
		  SDL_BlitSurface(images[IMG_EVILBILL_FLAME3 -
					flames[i].time / 4],
				  NULL, screen, &dest);
		}
	    }
	}
      
      
      /* (points): */
      
      for (i = 0; i < MAX_POINTS; i++)
	{
	  if (points[i].alive)
	    {
	      dest.x = points[i].x/2;
	      dest.y = points[i].y/2;
	      dest.w = 16;
	      dest.h = 8;
	      
	      SDL_BlitSurface(images[points[i].img], NULL, screen, &dest);
	    }
	}
      
      
      /* (message): */
      
      if (mesg_timer > 0)
	{
	  dest.x = (320 - (images[mesg_img] -> w)) / 2;
	  dest.y = 40;
	  dest.w = (images[mesg_img] -> w);
	  dest.h = (images[mesg_img] -> h);
	  
	  SDL_BlitSurface(images[mesg_img], NULL, screen, &dest);
	}
      
      
      /* (level bonus): */
      
      if (level_beat[player] > 0)
	{
	  /* "Level Bonus" text: */
	  
	  dest.x = (320 - (images[IMG_LEVEL_BONUS] -> w)) / 2;
	  dest.y = 40;
	  dest.w = images[IMG_LEVEL_BONUS] -> w;

	  src.x = 0;
	  src.y = 0;
	  src.w = images[IMG_LEVEL_BONUS] -> w;
	  
	  if (level_beat[player] < (images[IMG_LEVEL_BONUS] -> h))
	    {
	      dest.h = level_beat[player]/2;
	      src.h = level_beat[player]/2;
	    }
	  else
	    {
	      dest.h = images[IMG_LEVEL_BONUS] -> h;
	      src.h = images[IMG_LEVEL_BONUS] -> h;
	    }
	  
	  SDL_BlitSurface(images[IMG_LEVEL_BONUS], &src, screen, &dest);
	  
	  
	  /* Extra penguins: */
	  
	  for (i = 0; i < num_penguinoids && i < (level_beat[player] / 5); i++)
	    {
	      if (level_beat[player] < 170 - MAX_PENGUINOIDS ||
		  (level_beat[player] - (170 - MAX_PENGUINOIDS)) < i)
		{
		  dest.x = ((320 - (num_penguinoids * 16)) / 2) + (i * 16);
		  dest.y = 40 + (images[IMG_LEVEL_BONUS] -> h) + 8;
		  dest.w = 16;
		  dest.h = 16;
		  
		  SDL_BlitSurface(images[IMG_TUX_L0], NULL, screen, &dest);
		}
	    }
	  
	  
	  /* "No Bonus" */
	  
	  if (num_penguinoids == 0 &&
	      level_beat[player] > (images[IMG_LEVEL_BONUS] -> h) / 2)
	    {
	      dest.x = (320 - (images[IMG_NO_BONUS] -> w)) / 2;
	      dest.y = 40 + (images[IMG_LEVEL_BONUS] -> h) + 8;
	      dest.w = images[IMG_NO_BONUS] -> w;
	      dest.h = images[IMG_NO_BONUS] -> h;
	      
	      SDL_BlitSurface(images[IMG_NO_BONUS], NULL, screen, &dest);
	    }
	}


      /* (game over): */
      
      if (lives[player] < 0)
	{
	  /* "Game Over" text: */
	  
	  for (i = 0; i < (images[IMG_GAME_OVER] -> h); i++)
	    {
	      dest.x = (((320 - (images[IMG_GAME_OVER] -> w)) / 2) +
			(my_cos[(i + frame) % 16] * 2));
	      dest.y = ((240 - (images[IMG_GAME_OVER] -> h)) / 2) + i;
	      dest.w = images[IMG_GAME_OVER] -> w;
	      dest.h = 1;
	      
	      src.x = 0;
	      src.y = i;
	      src.w = images[IMG_GAME_OVER] -> w;
	      src.h = 1;
	      
	      SDL_BlitSurface(images[IMG_GAME_OVER], &src, screen, &dest);
	    }
	  
	  
	  /* Extra penguins: */
	  
	  for (i = 0; i < num_penguinoids && i < (level_beat[player] / 5); i++)
	    {
	      if (level_beat[player] < 170 - MAX_PENGUINOIDS ||
		  (level_beat[player] - (170 - MAX_PENGUINOIDS)) < i)
		{
		  dest.x = ((320 - (num_penguinoids * 16)) / 2) + (i * 16);
		  dest.y = 40 + (images[IMG_LEVEL_BONUS] -> h) + 8;
		  dest.w = 16;
		  dest.h = 16;
		  
		  SDL_BlitSurface(images[IMG_TUX_L0], NULL, screen, &dest);
		}
	    }
	  
	  
	  /* "No Bonus" */
	  
	  if (num_penguinoids == 0 &&
	      level_beat[player] > (images[IMG_LEVEL_BONUS] -> h) / 2)
	    {
	      dest.x = (320 - (images[IMG_NO_BONUS] -> w)) / 2;
	      dest.y = 40 + (images[IMG_LEVEL_BONUS] -> h) + 8;
	      dest.w = images[IMG_NO_BONUS] -> w;
	      dest.h = images[IMG_NO_BONUS] -> h;
	      
	      SDL_BlitSurface(images[IMG_NO_BONUS], NULL, screen, &dest);
	    }

	 if (score[0]>SaveData.points)
		  SaveData.points=score[0];
	  if (score[1]>SaveData.points)
		  SaveData.points=score[1];
	  if (level[0]>SaveData.level)
		  SaveData.level=level[0];
	  if (level[1]>SaveData.level)
		  SaveData.level=level[1];
	  saveData();

	}
      

      /* FLIP SCREEN! */
      
#ifdef SHOW_FPS
      draw_number(0, 240, last_time + (33 * SLOWDOWN) - SDL_GetTicks());
#endif /* #ifdef SHOW_FPS */
      
	sceDisplayWaitVblankStart(); 
      SDL_Flip(screen);
      
      
      /* Pause: */
      
      if (SDL_GetTicks() < last_time + (33 * SLOWDOWN))
	{
	  SDL_Delay(last_time + (33 * SLOWDOWN) - SDL_GetTicks());
	}
      
      
      /* Keep playing music: */
      
//#ifdef MUSIC
      if (use_sound == 1)
        {
          if (!Mix_PlayingMusic())
	    {
	      if (level[player] != 20)
		Mix_PlayMusic(game_musics[rand() % NUM_GAME_MUSICS], 0);
	      else
		{
		  if (num_aliens > 0)
		    Mix_PlayMusic(last_music, 0);
		  else
		    Mix_PlayMusic(win_music, 0);
		}

	      Mix_VolumeMusic(vol_music * (MIX_MAX_VOLUME / 5));
	    }
        }
//#endif /* #ifdef MUSIC */
    }
  while (done == 0 && quit == 0);
  
  
	//mode = title();

  /* Stop music and play game-over sound: */
  
//#ifdef SOUND
  if (use_sound == 1 && dancing[player] == 0)
    {
      Mix_HaltMusic();
      Mix_HaltChannel(-1);
      playsound(SND_GAMEOVER0 + (rand() % 3), 0, 1);
      
      do
	{
	  SDL_Delay(100);
	}
      while (Mix_Playing(0));
    }
//#endif /* #ifdef SOUND */
  
  
  return(quit);
}


/* Option screen loop: */

int option_screen(void)
{
  SDL_Event event;
  SDL_Rect dest;
  SDLKey key;
  Uint32 last_time;
  int done, quit, option_line, old_option_line, frame;
  
  option_line = 3;
  done = 0;
  quit = 0;
  
  dest.x = 58;
  dest.y = 53;
  dest.w = 218;
  dest.h = 136;
  
  SDL_FillRect(screen, &dest,
	       SDL_MapRGB(screen->format, 0x40, 0x40, 0x60));

  
  dest.x = 52;
  dest.y = 48;
  dest.w = 216;
  dest.h = 132;
  
  SDL_FillRect(screen, &dest,
	       SDL_MapRGB(screen->format, 0x80, 0x80, 0x80));


  dest.x = 54;
  dest.y = 60;
  dest.w = 212;
  dest.h = 118;
  
  SDL_FillRect(screen, &dest,
	       SDL_MapRGB(screen->format, 0x00, 0x00, 0x00));

  write_text(7, 5, "o ---- Options ------- =ox");

	sceDisplayWaitVblankStart(); 
  SDL_Flip(screen);
  
  write_text(7, 7, "EFFECTS VOLUME = ");
  write_num(24, 7, vol_effects);

  write_text(7, 8, "MUSIC VOLUME = ");
  write_num(24, 8, vol_music);

  write_text_inv(7, 17,
		 "# -- Port by chui ------ #");

  frame = 0;
  
  do
    {
      last_time = SDL_GetTicks();
      frame++;
      
      
      /* Check for keypresses: */
      
      old_option_line = option_line;
      
      while (SDL_PollEvent(&event))
	{
	  if (event.type == SDL_KEYDOWN)
	    {
	      key = event.key.keysym.sym;
	      
	      if (key == SDLK_ESCAPE)
		{
		  done = 1;
		}
	      else if (key == SDLK_UP)
		{
		  if (option_line > 3)
		    option_line--;
		}
	      else if (key == SDLK_DOWN)
		{
		  if (option_line < 4)
		    option_line++;
		}
	      else if (key == SDLK_LEFT)
	        {
		  if (option_line == 3)
		  {
		    if (vol_effects > 0)
		    {
		      vol_effects--;
//#ifdef SOUND
		      Mix_Volume(-1, vol_effects * (MIX_MAX_VOLUME / 5));
	              playsound(SND_SELECT, -1, 0);
//#endif
		    }
		  }
		  else if (option_line == 4)
		  {
		    if (vol_music > 0)
		    {
		      vol_music--;
//#ifdef SOUND
		      Mix_VolumeMusic(vol_music * (MIX_MAX_VOLUME / 5));
//#endif
		    }
		  }
	        }
	      else if (key == SDLK_RIGHT)
	        {
		  if (option_line == 3)
		  {
		    if (vol_effects < 5)
		    {
		      vol_effects++;
//#ifdef SOUND
		      Mix_Volume(-1, vol_effects * (MIX_MAX_VOLUME / 5));
	              playsound(SND_SELECT, -1, 0);
//#endif
		    }
		  }
		  else if (option_line == 4)
		  {
		    if (vol_music < 5)
		    {
		      vol_music++;
//#ifdef SOUND
		      Mix_VolumeMusic(vol_music * (MIX_MAX_VOLUME / 5));
//#endif
		    }
		  }
	        }
	      else if (key == SDLK_SPACE || key == SDLK_RETURN)
	        {
		  done = 1;
		}
	    }
//#ifdef JOY_YES
	  else if (event.type == SDL_JOYBUTTONDOWN &&
		   (num_joysticks < 2 || event.jbutton.which == player))
	    {
	      done = 1;
	    }
//#endif /* #ifdef JOY_YES */
	  else if (event.type == SDL_QUIT)
	    {
	      quit = 1;
	    }
	}

      
      if (old_option_line != option_line)
	{
	  dest.x = 48 * 4;
	  dest.y = (old_option_line + 4) * 10 - 1;
	  dest.w = 8;
	  dest.h = 10;
	  
	  SDL_FillRect(screen, &dest,
		       SDL_MapRGB(screen->format, 0, 0, 0));
	  write_num(24, old_option_line + 4, option_value(old_option_line));
sceDisplayWaitVblankStart(); 
	  SDL_Flip(screen);
	}
      
      
      /* Show current option: */
      
//      write_num(24, 7, option_line);
      
      if ((frame % 20) < 10)
	{
	  dest.x = 48 * 4;
	  dest.y = (option_line + 4) * 10 - 1;
	  dest.w = 8;
	  dest.h = 10;
	  
	  SDL_FillRect(screen, &dest,
		       SDL_MapRGB(screen->format, 255, 255, 255));
	sceDisplayWaitVblankStart(); 
	  SDL_Flip(screen);
	}
      else
	{
	  dest.x = 48 * 4;
	  dest.y = ((option_line + 4) * 10) - 1;
	  dest.w = 8;
	  dest.h = 10;
	  
	  SDL_FillRect(screen, &dest,
		       SDL_MapRGB(screen->format, 0, 0, 0));
	  write_num(24, option_line + 4, option_value(option_line));
	sceDisplayWaitVblankStart(); 
	  SDL_Flip(screen);
	}
      

      /* Keep playing music: */
      
//#ifdef MUSIC
      if (use_sound == 1)
        {
          if (!Mix_PlayingMusic())
	    {
              Mix_PlayMusic(title_music, 0);
	      Mix_VolumeMusic(vol_music * (MIX_MAX_VOLUME / 5));
	    }
        }
//#endif /* #ifdef MUSIC */
      
      
      /* Pause til next frame: */
      
      if (SDL_GetTicks() < last_time + 33)
	SDL_Delay(last_time + 33 - SDL_GetTicks());
    }
  while (!done && !quit);
  
  return(quit);
}



/* Title loop: */

int title(void)
{
  SDL_Rect dest, src;
  int done, i, option, old_option, count, impatience_count,
    angle, angle_m, angle_mm, angle_time, num_starfields, distortion;
  int old_axis;
  Uint32 last_time;
  SDL_Event event;
  SDLKey key;
  
  /* Reset starfields: */
  
  for (i = 0; i < MAX_STARFIELDS; i++)
    {
      starfields[i].radius = (rand() % 320) + 1;
      starfields[i].angle = (rand() % 360);
      starfields[i].time = (rand() % 32);
    }
  
  angle = 0;
  angle_m = 0;
  angle_mm = 0;
  angle_time = (rand() % 100) + 100;
  
  num_starfields = 1;
  
  
  /* MAIN TITLE SCREEN LOOP: */
  
  count = 0;
  done = 0;
  option = TITLE_OPTION_ONE_PLAYER;
  old_option = option;
  impatience_count = 0;
  distortion = 128;
  old_axis = 0;
  
  do
    {
      last_time = SDL_GetTicks();
      
      
      /* Check for keypresses: */
      
      while (SDL_PollEvent(&event))
	{
	  if (event.type == SDL_KEYDOWN)
	    {
	      key = event.key.keysym.sym;
	      
	      if (key == SDLK_ESCAPE)
		{
		  done = 1;
		  option = TITLE_OPTION_QUIT;
		}
	      else if (key == SDLK_UP)
		{
		  if (option > 0)
		    option--;
		}
	      else if (key == SDLK_DOWN)
		{
		  if (option < NUM_TITLE_OPTIONS - 1)
		    option++;
		}
	      else if (key == SDLK_SPACE || key == SDLK_RETURN)
		done = 1;
	    }
//#ifdef JOY_YES
	  else if (event.type == SDL_JOYAXISMOTION &&
		   event.jaxis.axis == JOY_Y)
	    {
		
	      if (event.jaxis.value < -256 && old_axis >= -128)
		{
		  if (option > 0)
		  	option--;
		}
	      else if (event.jaxis.value > 256 && old_axis <= 128)
		{
		 if (option < NUM_TITLE_OPTIONS - 1)
		 	option++;
		}
	      
	      old_axis = event.jaxis.value;
	    }
	  else if (event.type == SDL_JOYBUTTONDOWN &&
		   (num_joysticks < 2 || event.jbutton.which == player))
	    {
	      done = 1;
	    }
//#endif /* #ifdef JOY_YES */
	  else if (event.type == SDL_QUIT)
	    {
	      done = 1;
	      option = TITLE_OPTION_QUIT;
	    }
	}
      
      
      /* Move stars: */
      
      for (i = 0; i < MAX_STARFIELDS; i++)
	{
	  starfields[i].radius = (starfields[i].radius +
				  ((32 - starfields[i].time) / 2));
	  starfields[i].time--;
	  
	  if (starfields[i].radius >= 640 || starfields[i].time <= 0)
	    {
	      starfields[i].radius = (rand() % 320) + 1;
	      starfields[i].angle = (rand() % 360);
	      starfields[i].time = 32;
	    }
	}
      
      
      /* Rotate stars: */
      
      angle = angle + angle_m;
      if (angle >= 360)
	angle = angle - 360;
      else if (angle <= 0)
	angle = angle + 360;
      
      if ((count % 4) == 0)
	angle_m = angle_m + angle_mm;

      angle_time--;
      
      
      /* Change rotation? */
      
      if (angle_time <= 0 || angle_m < -5 || angle_m > 5)
	{
	  if (angle_m < -5)
	    {
	      angle_m = -5;
	      angle_mm = (rand() % 2);
	    }
	  else if (angle_m > 5)
	    {
	      angle_m = 5;
	      angle_mm = -(rand() % 2);
	    }
	  else
	    angle_mm = (rand() % 4) - 2;

	  angle_time = (rand() % 100) + 100;
	}
      
      if (num_starfields < MAX_STARFIELDS)
	num_starfields++;
      
      
      /* Draw screen: */
      
      /* (Black): */
      
      SDL_FillRect(screen, NULL,
		   SDL_MapRGB(screen->format, 0x00, 0x00, 0x00));
      
      /* Draw the last game's scores: */
      {
	  static cuenta=0;
	  if ((cuenta%50)>25)
	  {
	  	write_text(0,22, "TOP LEVEL: ");
	 	write_num(10, 22, SaveData.level);
	  	write_text(0,23, "HI SCORE: ");
	  	write_num(10, 23, SaveData.points);
	  }
	  cuenta++;
      }
      
      /* (Starfield): */
      
      for (i = 0; i < num_starfields; i++)
	{
	  dest.x = ((cos(M_PI * (angle + starfields[i].angle) / 180) *
		    (starfields[i].radius)) + 320)/2;
	  dest.y = ((-sin(M_PI * (angle + starfields[i].angle) / 180) *
		    (starfields[i].radius)) + 240)/2;
	  dest.w = 2;
	  dest.h = 1;
	  
	  src.x = 0;
	  src.y = starfields[i].time;
	  src.w = 2;
	  src.h = 1;
	  
	  SDL_BlitSurface(images[IMG_TITLE_STARFIELD], &src, screen, &dest);
	}
      
      
      /* (Title gfx): */
      
      if (distortion < 2)
	{
	  dest.x = (320 - (images[IMG_TITLE_TITLE] -> w)) / 2;
	  dest.y = 0;
	  dest.w = images[IMG_TITLE_TITLE] -> w;
	  dest.h = images[IMG_TITLE_TITLE] -> h;
	  
	  SDL_BlitSurface(images[IMG_TITLE_TITLE], NULL, screen, &dest);
	}
      else
	{
	  for (i = (count % (distortion / 2));
	       i < images[IMG_TITLE_TITLE] -> h;
	       i = i + (distortion / 2))
	    {
	      dest.x = (((320 - (images[IMG_TITLE_TITLE] -> w)) / 2) +
			my_cos[(i + count) % 32] * distortion);
	      dest.y = i;
	      dest.w = images[IMG_TITLE_TITLE] -> w;
	      dest.h = 1;
	      
	      src.x = 0;
	      src.y = i;
	      src.w = images[IMG_TITLE_TITLE] -> w;
	      src.h = 1;
	      
	      SDL_BlitSurface(images[IMG_TITLE_TITLE], &src, screen, &dest);
	    }
	  
	  distortion--;
	}
      
      
      /* (Menu items): */
      
      for (i = 0; i < NUM_TITLE_OPTIONS; i++)
	{
	  dest.x = 112;
	  dest.y = (images[IMG_TITLE_TITLE] -> h) + ((i + 3) * 16);
	  dest.w = images[title_option_images[i]] -> w;
	  dest.h = 16;
	  
	  SDL_BlitSurface(images[title_option_images[i]], NULL, screen, &dest);
	}
      
  
      /* Play selection noise: */
      
      if (option != old_option)
	{
	  old_option = option;
	  playsound(SND_SELECT, -1, 0);
	}
      
      
      /* Animate the arrow: */
      
      count++;

      /* Draw the option selection arrow: */
      
      dest.x = (192/2);
      dest.y = (images[IMG_TITLE_TITLE] -> h) + ((option + 3) * 16);
      dest.w = 16;
      dest.h = 16;
      
      SDL_BlitSurface(images[IMG_TITLE_ARROW0 + (count / 10) % 3],
		      NULL, screen, &dest);

      sceDisplayWaitVblankStart(); 
      SDL_Flip(screen);
      
      
      /* Keep playing music: */
      
//#ifdef MUSIC
      if (use_sound == 1)
        {
          if (!Mix_PlayingMusic())
	    {
              Mix_PlayMusic(title_music, 0);
	      Mix_VolumeMusic(vol_music * (MIX_MAX_VOLUME / 5));
	    }
        }
//#endif /* #ifdef MUSIC */
      
      
      impatience_count++;
      
      if (impatience_count >= 600)
	{
	  impatience_count = 0;
	  playsound(SND_IMPATIENCE, 0, 1);
	}
      

      /* Pause til next frame: */
      if (SDL_GetTicks() < last_time + 33)
	SDL_Delay(last_time + 33 - SDL_GetTicks());
    }
  while (done == 0);
  

  playsound(SND_CONFIRM, 0, 1);
  
  SDL_Delay(200);
  
  return (option);
}

static inline void draw_percent(int i)
{
	int col;
	SDL_Rect dest;

	      // Draw percentage bar
  
		dest.x = 0;
		dest.y = 216;
		dest.w = (320 * i) / (NUM_IMAGES+NUM_SOUNDS+((NUM_GAME_MUSICS+3)*5));
		dest.h = 24;

		col = 127+((128 * i) / (NUM_IMAGES+NUM_SOUNDS+((NUM_GAME_MUSICS+3)*5)));

		SDL_FillRect(screen, &dest, SDL_MapRGB(screen->format, col, col, 255));
}

static inline void draw_loading(int i)
{
	SDL_Rect dest;
	
      if (i == 0)
	{
	  /* Draw "loading" background: */
	  
	  SDL_BlitSurface(images[IMG_LOADING], NULL, screen, NULL);
	sceDisplayWaitVblankStart(); 
	  SDL_Flip(screen);
	}
      else 
	{
		dest.x = 0;
		dest.y = 240 - (images[1] -> h);
		dest.w = 320;
		dest.h = images[1] -> h;
		SDL_BlitSurface(images[IMG_LOADING], &dest, screen, &dest);
		draw_percent(i);
//		if (i%2)
		{
			dest.y += 2;
			dest.x = 160 - ((images[1]-> w)/2);
			dest.w = images[1] -> w;
			SDL_BlitSurface(images[IMG_LOADING_TEXT], NULL, screen, &dest);
			dest.y -= 2;
		}

		SDL_UpdateRect(screen, 0, dest.y, 320, dest.h);
	}

}


/* Setup: */

void setup(void)
{
  int i, done, distortion, distortion_m;
  Uint32 old_ticks, last_time;
  SDL_Surface * image;
  SDL_Rect dest, src;
  SDL_Event event;
  
  
  /* Init SDL Video: */
  
  if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
      fprintf(stderr,
              "\nError: I could not initialize video!\n"
              "The Simple DirectMedia error that occured was:\n"
              "%s\n\n", SDL_GetError());
      exit(1);
    }
  
  
  /* Open display: */
  
  //screen = SDL_SetVideoMode(320,240,0,SDL_HWSURFACE|SDL_HWPALETTE);
	screen = SDL_SetVideoMode(480, 272, 32, 0);
  if (screen == NULL)
        {
          fprintf(stderr,
                  "\nError: I could not set up video for 320x240 mode.\n"
                  "The Simple DirectMedia error that occured was:\n"
                  "%s\n\n", SDL_GetError());
          exit(1);
        }
  
  
//#ifdef SOUND
  /* Open sound: */
  
     if (Mix_OpenAudio(22050, AUDIO_S16, 2, 512) < 0)
        {
          fprintf(stderr,
                  "\nWarning: I could not set up audio for 22050 Hz "
                  "16-bit stereo.\n"
                  "The Simple DirectMedia error that occured was:\n"
                  "%s\n\n", SDL_GetError());
	  exit(2);
        }

  vol_music=SaveData.music;
  vol_effects=SaveData.sound;
  
  for (i = 0; i < 4; i++)
    dontblockchan[i] = 0;
  
  Mix_Volume(-1, vol_effects * (MIX_MAX_VOLUME / 5));
  Mix_VolumeMusic(vol_music * (MIX_MAX_VOLUME / 5));
//#endif /* #ifdef SOUND */


  use_joystick = 0;
  num_joysticks = 0;
//#ifdef JOY_YES
  use_joystick = 1;
  
  
  /* Init joystick: */
  
  if (SDL_Init(SDL_INIT_JOYSTICK) < 0)
    {
      fprintf(stderr,
	      "\nWarning: I could not initialize joystick.\n"
	      "The Simple DirectMedia error that occured was:\n"
	      "%s\n\n", SDL_GetError());
      
      use_joystick = 0;
    }
  else
    {
      /* Look for joysticks: */
      
      num_joysticks = SDL_NumJoysticks();
      
      if (num_joysticks <= 0)
	{
	  fprintf(stderr,
		  "\nWarning: No joysticks available.\n");
	  
	  use_joystick = 0;
	}
      else
	{
	  /* Open joystick: */
	  
	  js = SDL_JoystickOpen(0);
	  
	  if (js == NULL)
	    {
	      fprintf(stderr,
		      "\nWarning: Could not open joystick 1.\n"
		      "The Simple DirectMedia error that occured was:\n"
		      "%s\n\n", SDL_GetError());
	      
	      use_joystick = 0;
	    }
	  else
	    {
	      /* Check for proper stick configuration: */
	      
	      if (SDL_JoystickNumAxes(js) < 2)
		{
		  fprintf(stderr,
			  "\nWarning: Joystick doesn't have enough axes!\n");
		  
		  use_joystick = 0;
		}
	      else
		{
		  if (SDL_JoystickNumButtons(js) < 2)
		    {
		      fprintf(stderr,
			      "\nWarning: Joystick doesn't have enough "
			      "buttons!\n");
		      
		      use_joystick = 0;
		    }
		}
	    }
	}
    }
//#endif /* JOY_YES */
  
  
  /* Set window manager stuff: */
  
  SDL_WM_SetCaption("Defendguin", "Defendguin");
  
  
  /* Load graphics: */
  
  old_ticks = (Uint32) 0;
  flash = 0;
  
  for (i = 0; i < NUM_IMAGES; i++)
    {
//#ifdef DEBUG
//#ifndef GP32
      printf("Loading %s\n",image_names[i]);
//#endif
//#endif
//#ifdef GP32
      image = IMG_Load_RW(get_data(image_names[i]),0);
//#else
//      image = IMG_Load(image_names[i]);
//#endif
//#ifdef DEBUG
//      puts("Ok.");
//#endif
      
      if (image == NULL)
	{
//#ifdef DEBUG
	  fprintf(stderr,
		  "\nError: I couldn't load a graphics file:\n"
		  "%s\n"
		  "The Simple DirectMedia error that occured was:\n"
		  "%s\n\n", image_names[i], SDL_GetError());
//#endif
	  exit(1);
	}
      
      
      /* Convert to display format: */
  
      images[i] = SDL_DisplayFormat(image);
      if (images[i] == NULL)
        {
//#ifdef DEBUG
          fprintf(stderr,
                  "\nError: I couldn't convert a file to the display format:\n"
                  "%s\n"
                  "The Simple DirectMedia error that occured was:\n"
                  "%s\n\n", image_names[i], SDL_GetError());
//#endif
          exit(1);
        }

      
      /* Set transparency: */
      
      if (i != 0)
	{
	  if (SDL_SetColorKey(images[i], (SDL_SRCCOLORKEY | SDL_RLEACCEL),
			      SDL_MapRGB(images[i] -> format, 0xFF, 0xFF, 0xFF)) == -1)
	    {
//#ifdef DEBUG
	      fprintf(stderr,
		      "\nError: I could not set the color key for the file:\n"
		      "%s\n"
		      "The Simple DirectMedia error that occured was:\n"
		      "%s\n\n", image_names[i], SDL_GetError());
//#endif
	      exit(1);
	    }
	}
      
      draw_loading(i);


      
      SDL_FreeSurface(image);
    }

//#ifdef SOUND
  
      /* Load sounds: */
      
      for (i = 0; i < NUM_SOUNDS; i++)
	{
//#ifdef GP32
	  sounds[i] = Mix_LoadWAV_RW(get_data(sound_names[i]),1);
//#else
//	  sounds[i] = Mix_LoadWAV(sound_names[i]);
//#endif
	  if (sounds[i] == NULL)
	    {
//#ifdef DEBUG
	      fprintf(stderr,
		      "\nError: I could not load the sound file:\n"
		      "%s\n"
		      "The Simple DirectMedia error that occured was:\n"
		      "%s\n\n", sound_names[i], SDL_GetError());
//#endif
	      exit(1);
	    }

	  draw_loading(i+NUM_IMAGES);
	}
      
//#ifdef MUSIC      
      /* Load music: */
      
      title_music = Mix_LoadMUS(MUS_TITLE);
      if (title_music == NULL)
        {
//#ifdef DEBUG
          fprintf(stderr,
                  "\nError: I could not load the music file:\n"
                  "%s\n"
                  "The Simple DirectMedia error that occured was:\n"
                  "%s\n\n", MUS_TITLE, SDL_GetError());
//#endif
          exit(1);
        }

      draw_loading(NUM_IMAGES+NUM_SOUNDS);

      last_music = Mix_LoadMUS(MUS_LAST);
      if (last_music == NULL)
        {
//#ifdef DEBUG
          fprintf(stderr,
                  "\nError: I could not load the music file:\n"
                  "%s\n"
                  "The Simple DirectMedia error that occured was:\n"
                  "%s\n\n", MUS_LAST, SDL_GetError());
//#endif
          exit(1);
        }
      draw_loading(5+NUM_IMAGES+NUM_SOUNDS);


      win_music = Mix_LoadMUS(MUS_WIN);
      if (win_music == NULL)
        {
//#ifdef DEBUG
          fprintf(stderr,
                  "\nError: I could not load the music file:\n"
                  "%s\n"
                  "The Simple DirectMedia error that occured was:\n"
                  "%s\n\n", MUS_WIN, SDL_GetError());
//#endif
          exit(1);
        }
      draw_loading(10+NUM_IMAGES+NUM_SOUNDS);
      
      
      for (i = 0; i < NUM_GAME_MUSICS; i++)
        {
          game_musics[i] = Mix_LoadMUS(game_music_names[i]);
          if (game_musics[i] == NULL)
            {
//#ifdef DEBUG
              fprintf(stderr,
                      "\nError: I could not load the music file:\n"
                      "%s\n"
                      "The Simple DirectMedia error that occured was:\n"
                      "%s\n\n", game_music_names[i], SDL_GetError());
//#endif
              exit(1);
            }
      	draw_loading(15+(i*5)+NUM_IMAGES+NUM_SOUNDS);
        }
//#endif  

      Mix_PlayMusic(title_music, 0);
      Mix_VolumeMusic(vol_music * (MIX_MAX_VOLUME / 5));
//#endif /* #ifdef SOUND */
  
  
  /* Erase "Loading": */
  
  dest.x = (320 - (images[1] -> w)) / 2;
  dest.y = 240 - (images[1] -> h);
  dest.w = images[1] -> w;
  dest.h = images[1] -> h;
  
  SDL_BlitSurface(images[IMG_LOADING], &dest, screen, &dest);
  SDL_UpdateRect(screen, dest.x, dest.y, dest.w, dest.h);
  
  
  /* Initialize random seed: */
  
  srand(SDL_GetTicks());
  
  
  /* Initialize cosine wave: */
  
  for (i = 0; i < 32; i++)
    {
      my_cos[i] = cos(M_PI * (i * 11) / 180);
    }
  
  
  /* Draw some informative text: */
  
  write_centered_text(33, "PORT BY CHUI");


  /* Flash "Press a key" on the screen (yes, I'm _really_ proud of the
     loading screen art! :) ) */
  
  done = 0;
  distortion = 0;
  distortion_m = 1;
  
  do
    {
      last_time = SDL_GetTicks();
      
      
      /* Check for keypresses: */
      
      while (SDL_PollEvent(&event))
	{
	  if (event.type == SDL_KEYDOWN)
	    done = 1;
	  
//#ifdef JOY_YES
	  if (event.type == SDL_JOYBUTTONDOWN &&
	      (num_joysticks < 2 || event.jbutton.which == player))
	    {
	      done = 1;
	    }
//#endif /* #ifdef JOY_YES */
	}
      
      
      dest.x = 0;
      dest.y = 240 - (images[IMG_PRESS_A_KEY] -> h);
      dest.w = 320;
      dest.h = images[IMG_PRESS_A_KEY] -> h;
      
      SDL_BlitSurface(images[0], &dest, screen, &dest);
      
      
      /* Draw fuzzy text behind: */
      
      distortion = distortion + distortion_m;
      if (distortion == 0)
	distortion_m = 1;
      else if (distortion == 64)
	distortion_m = -1;
      
      for (i = (flash % 2); i < images[IMG_PRESS_A_KEY] -> h; i = i + 2)
	{
	  dest.x = (((320 - (images[IMG_PRESS_A_KEY] -> w)) / 2) +
		    my_cos[(i + flash) % 32] * distortion);
	  dest.y = 240 - (images[IMG_PRESS_A_KEY] -> h) + i;
	  dest.w = images[IMG_PRESS_A_KEY] -> w;
	  dest.h = 1;
	  
	  src.x = 0;
	  src.y = i;
	  src.w = images[IMG_PRESS_A_KEY] -> w;
	  src.h = 1;
	  
	  SDL_BlitSurface(images[IMG_PRESS_A_KEY], &src, screen, &dest);
	}
      
      
      /* Flash the image: */
      
      flash++;
      
      dest.x = (320 - (images[IMG_PRESS_A_KEY] -> w)) / 2;
      dest.y = 240 - (images[IMG_PRESS_A_KEY] -> h);
      dest.w = images[IMG_PRESS_A_KEY] -> w;
      dest.h = images[IMG_PRESS_A_KEY] -> h;
      
      if (flash < 15)
	SDL_BlitSurface(images[IMG_PRESS_A_KEY], NULL, screen, &dest);
      else if (flash == 30)
	flash = 0;
      
      SDL_UpdateRect(screen, 0, dest.y, 320, dest.h);
      
      
      /* Pause til next frame: */
      
      if (SDL_GetTicks() < last_time + 33)
	SDL_Delay(last_time + 33 - SDL_GetTicks());
      
//#ifdef MUSIC
      if (use_sound == 1)
	{
	  if (Mix_PlayingMusic() == 0)
	    {
	      Mix_PlayMusic(title_music, 0);
	      Mix_VolumeMusic(vol_music * (MIX_MAX_VOLUME / 5));
	    }
	}
//#endif /* #ifdef MUSIC */
    }
  while (done == 0);
}


/* Quit! */

void my_shutdown(void)
{
//#ifdef SOUND
  if (use_sound == 1)
    {
      Mix_HaltMusic();
      Mix_HaltChannel(-1);
    }
//#endif /* #ifdef SOUND */
  
  SDL_Quit();
}


/* Create landscape: */

void create_land(int ply)
{
  int x, y, height, old_height, first_height;
  
  old_height = 0;
  height = (rand() % (LAND_HEIGHT - LAND_MIN_HEIGHT) + LAND_MIN_HEIGHT);
  first_height = height;
  
  for (x = 0; x < LAND_WIDTH - (LAND_HEIGHT / 2); x++)
    {
      for (y = 0; y < LAND_HEIGHT; y++)
	{
	  if (height < y)
	    land[ply][x][y] = IMG_LAND_CENTER;
	  else if (height == y)
	    {
	      if (height > old_height)
		{
		  land[ply][x][y - 1] = IMG_LAND_RIGHT;
		  land[ply][x][y] = IMG_LAND_CENTER;
		}
	      else if (height < old_height)
		land[ply][x][y] = IMG_LAND_LEFT;
	      else
		land[ply][x][y] = IMG_LAND_CENTER;
	    }
	  else
	    land[ply][x][y] = -1;
	}
      
      old_height = height;
      
      if ((rand() % 3) != 0)
	{
	  if ((rand() % 2) == 0)
	    height++;
	  else
	    height--;
	}
      
      if (height < LAND_MIN_HEIGHT)
	height = height + 2;
      else if (height >= LAND_HEIGHT)
	height = height - 2;
    }
  
  for (x = LAND_WIDTH - (LAND_HEIGHT / 2); x < LAND_WIDTH; x++)
    {
      for (y = 0; y < LAND_HEIGHT; y++)
	{
	  if (height < y)
	    land[ply][x][y] = IMG_LAND_CENTER;
	  else if (height == y)
	    {
	      if (height > old_height)
		{
		  land[ply][x][y - 1] = IMG_LAND_RIGHT;
		  land[ply][x][y] = IMG_LAND_CENTER;
		}
	      else if (height < old_height)
		land[ply][x][y] = IMG_LAND_LEFT;
	      else
		land[ply][x][y] = IMG_LAND_CENTER;
	    }
	  else
	    land[ply][x][y] = -1;
	}
      
      old_height = height;
      
      if (height > first_height - 1)
	height--;
      else if (height < first_height - 1)
	height++;
    }
}


/* Create friendly penguinoids: */

void create_penguinoids(int ply, int num)
{
  int i;
  
  for (i = 0; i < MAX_PENGUINOIDS; i++)
    {
      if (i < num)
	{
	  penguinoids[ply][i].alive = 1;
	  penguinoids[ply][i].mode = PENG_MODE_WALKING;
	  penguinoids[ply][i].being_homed = 0;
	  penguinoids[ply][i].x = (rand() % (LAND_WIDTH * 32));
	  penguinoids[ply][i].y = 480 - 32;

	  penguinoids[ply][i].xm = 1;
	  if ((rand() % 2) == 0)
	    penguinoids[ply][i].xm = -penguinoids[ply][i].xm;

	  penguinoids[ply][i].ym = 1;
	  if ((rand() % 2) == 0)
	    penguinoids[ply][i].ym = -penguinoids[ply][i].ym;
	}
      else
	penguinoids[ply][i].alive = 0;
    }
}


/* Draw a number on the screen: */

void draw_number(int x, int y, int n)
{
  char str[16];
  SDL_Rect src, dest;
  int i;
  
  
  if (n == 0)
    strcpy(str, "    00");
  else
    sprintf(str, "%6d", n);
  
  for (i = 0; i < strlen(str); i++)
    {
      dest.x = x + i * 12;
      dest.y = y;
      dest.w = 12;
      dest.h = 16;
      
      src.x = (str[i] - '0') * 12;
      src.y = 0;
      src.w = 12;
      src.h = 16;
      
      SDL_BlitSurface(images[IMG_NUMBERS], &src, screen, &dest);
    }
}


/* Add an alien: */

void add_alien(int x, int y, int type, int mode, int timer)
{
  int i, found;
  
  
  /* Look for a slot in the aliens array: */
  
  found = -1;
  
  for (i = 0; i < MAX_ALIENS && found == -1; i++)
    {
      if (aliens[player][i].alive == 0)
	found = i;
    }
  
  
  /* Pick random spots, if x and/or y are set to -1: */
  
  if (x == -1)
    x = (rand() % (LAND_WIDTH * 32));
  
  if (y == -1)
    y = (rand() % 240) + 80;

  
  /* Add the mine! */
  
  if (found != -1)
    {
      aliens[player][found].alive = 1;
      
      if (planet_dead[player] == 0 ||
	  type == ALIEN_MINE || type == ALIEN_BAITER ||
	  type == ALIEN_EVILBILL)
	aliens[player][found].type = type;
      else
	aliens[player][found].type = ALIEN_MUTANT;
      
      aliens[player][found].mode = mode;
      aliens[player][found].shields = 20;
      
      aliens[player][found].x = x;
      aliens[player][found].y = y;
      
      aliens[player][found].xm = (rand() % 4) + 1;
      if ((rand() % 2) == 0)
	aliens[player][found].xm = -aliens[player][found].xm;
      
      aliens[player][found].xmm = 1;
      
      aliens[player][found].ym = (rand() % 4) + 1;
      if ((rand() % 2) == 0)
	aliens[player][found].ym = -aliens[player][found].ym;
      
      aliens[player][found].ymm = 1;
      
      aliens[player][found].ym = 0;
      aliens[player][found].timer = timer;
    }
}


/* Create stars: */

void create_stars(void)
{
  int i;
  
  for (i = 0; i < MAX_STARS; i++)
    {
      stars[i].x = (rand() % (LAND_WIDTH * 16));
      stars[i].y = (rand() % 400) + 80;
    }
}


/* Add a laser shot: */

void add_laser(int x, int y, int dir)
{
  int i, found;
  
  
  /* Find a free laser slot: */

  found = -1;
  
  for (i = 0; i < MAX_LASERS && found == -1; i++)
    {
      if (lasers[i].alive == 0)
	found = i;
    }
  
  
  /* Turn on the laser shot: */
  
  if (found != -1)
    {
      lasers[found].alive = 1;
      
      
      if (dir == DIR_RIGHT)
	x = x + 32 - LASER_SPEED;
      else if (dir == DIR_LEFT)
	x = x + 32 + LASER_SPEED;
      
      lasers[found].x1 = x;
      
      if (dir == DIR_RIGHT)
	lasers[found].x2 = 640;
      else
	lasers[found].x2 = 0;
      
      lasers[found].y = y;
    }  
}


/* Play a sound: */

void playsound(int snd, int chan, int blocking)
{
//#ifdef SOUND
  if (use_sound == 1)
    {
      if (chan == -1 || blocking == 1 ||
	  Mix_Playing(chan) == 0 || dontblockchan[chan] == 0)
	{
	  Mix_PlayChannel(chan, sounds[snd], 0);
	  dontblockchan[chan] = blocking;
	}
    }
//#endif /* #ifdef SOUND */
}


/* Add an explosion: */

void add_explosion(int x, int y, int img)
{
  add_explosion_bit(x, y, 0, -4, img);
  add_explosion_bit(x, y, 2, -2, img);
  add_explosion_bit(x, y, 4, 0, img);
  add_explosion_bit(x, y, 2, 2, img);
  add_explosion_bit(x, y, 0, 4, img);
  add_explosion_bit(x, y, -2, 2, img);
  add_explosion_bit(x, y, -4, 0, img);
  add_explosion_bit(x, y, -2, -2, img);
}


/* Add a bit of explosion: */

void add_explosion_bit(int x, int y, int xm, int ym, int img)
{
  int i, found;
  
  /* Find an explosion slot: */
  
  found = -1;
  
  for (i = 0; i < MAX_EXPLOSION_BITS && found == -1; i++)
    {
      if (explosion_bits[i].alive == 0)
	found = i;
    }
  
  
  /* Add the bit: */
  
  if (found != -1)
    {
      explosion_bits[found].alive = 1;
      explosion_bits[found].x = x;
      explosion_bits[found].y = y;
      explosion_bits[found].xm = xm;
      explosion_bits[found].ym = ym;
      explosion_bits[found].time = 50;
      explosion_bits[found].img = img;
    }
}


/* Add points: */

void add_points(int xx, int yy, int img)
{
  int i, found, oldest, oldest_time, realx;
  
  
  /* Find a free (or the oldest) slot: */
  
  found = -1;
  oldest = 0;
  oldest_time = 1000;
  
  for (i = 0; i < MAX_POINTS && found == -1; i++)
    {
      if (points[i].alive == 0)
	found = i;
      
      if (points[i].time < oldest_time)
	{
	  oldest_time = points[i].time;
	  oldest = i;
	}
    }
  
  
  /* If there was no free, pick the oldest slot: */
  
  if (found == -1)
    found = oldest;
  
  
  /* Determine a screen position: */
  
  realx = xx - (x[player] - scroll[player]);
  
  if (realx < -31)
    realx = realx + (LAND_WIDTH * 32);
  
  while (realx >= (LAND_WIDTH * 32))
    realx = realx - (LAND_WIDTH * 32);
  
  
  /* Turn on the points bit: */

  points[found].alive = 1;
  points[found].x = realx;
  points[found].y = yy;
  points[found].time = 30;
  points[found].img = img;
}


/* Add flame: */

void add_flame(int x, int y)
{
  int i, found;
  
  
  /* Find a free slot: */
  
  found = -1;
  
  for (i = 0; i < MAX_FLAMES && found == -1; i++)
    {
      if (flames[i].alive == 0)
	found = i;
    }
  
  
  /* Turn on the flame: */
  
  if (found != -1)
    {
      flames[found].alive = 1;
      flames[found].x = x;
      flames[found].y = y;
      flames[found].xm = (rand() % 10) - 5;
      flames[found].time = 11;
    }
}


/* Kill a particular alien (add points, explosion, release penguinoids): */

void kill_alien(int i)
{
  int j;
  
  if (aliens[player][i].type != ALIEN_EVILBILL ||
      aliens[player][i].y >= 480)
    {
      aliens[player][i].alive = 0;
      
      j = IMG_LASERS;
      
      if (aliens[player][i].type == ALIEN_UFO)
	j = IMG_UFO0;
      else if (aliens[player][i].type == ALIEN_MUTANT)
	j = IMG_LASERS;
      else if (aliens[player][i].type == ALIEN_POD)
	j = IMG_POD0;
      else if (aliens[player][i].type == ALIEN_SWARMER)
	j = IMG_SWARMER;
      else if (aliens[player][i].type == ALIEN_BOMBER)
	j = IMG_BOMBER;
      
      add_explosion(aliens[player][i].x + 16, aliens[player][i].y + 16, j);
      
      playsound(SND_EXPLODE, 3, 0);
      
      
      /* How many points is it worth? */
      
      if (aliens[player][i].type == ALIEN_UFO ||
	  aliens[player][i].type == ALIEN_MUTANT ||
	  aliens[player][i].type == ALIEN_SWARMER)
	{
	  add_score(150);
	  add_points(aliens[player][i].x, aliens[player][i].y,
		     IMG_150);
	}
      else if (aliens[player][i].type == ALIEN_BOMBER)
	{
	  add_score(250);
	  add_points(aliens[player][i].x, aliens[player][i].y,
		     IMG_250);
	}
      else if (aliens[player][i].type == ALIEN_BAITER)
	{
	  add_score(200);
	  add_points(aliens[player][i].x, aliens[player][i].y,
		     IMG_200);
	}
      else if (aliens[player][i].type == ALIEN_POD)
	{
	  add_score(1000);
	  add_points(aliens[player][i].x, aliens[player][i].y,
		     IMG_1000);
	}
      
      
      /* If it was a UFO and was beaming a penguin,
	 release the penguin: */
      
      if (aliens[player][i].mode == ALIEN_MODE_HOME_PENG ||
	  aliens[player][i].mode == ALIEN_MODE_CAPTURED_PENG ||
	  aliens[player][i].mode == ALIEN_MODE_BEAMING_PENG)
	{
	  penguinoids[player][aliens[player][i].home_peng].mode =
	    PENG_MODE_FALLING;
	  penguinoids[player][aliens[player][i].home_peng].being_homed = 0;
	  penguinoids[player][aliens[player][i].home_peng].ym = 1;
	  set_message(IMG_CATCH_THE_PENGUINOID);
	}
      
      
      /* Was it Evil Bill? */
      
      if (aliens[player][i].type == ALIEN_EVILBILL)
	{
	  /* Do an interesting implosion effect: */
	  
	  for (j = 0; j < MAX_EXPLOSION_BITS; j++)
	    {
	      if (explosion_bits[j].alive)
		{
		  explosion_bits[j].xm = -explosion_bits[j].xm;
		  explosion_bits[j].ym = -explosion_bits[j].ym;
		}
	    }
	  
	  
	  /* Get ready for winning music: */
	  
//#ifdef SOUND
	  if (use_sound == 1)
	    {
	      Mix_HaltMusic();
	    }
//#endif /* #ifdef SOUND */
	  
	  
	  /* Kill all other aliens (mines): */
	  
	  for (j = 0; j < MAX_ALIENS; j++)
	    {
	      aliens[player][j].alive = 0;
	    }
	}
    }
  else
    {
      /* Hurt Evil Bill: */
      
      aliens[player][i].mode = ALIEN_MODE_HURTING;
      aliens[player][i].timer = 10;
      
      aliens[player][i].shields--;
      
      
      if (aliens[player][i].shields <= 0)
	{
	  /* Shields dead!  Kill! */
	  
	  aliens[player][i].mode = ALIEN_MODE_DYING;
	  aliens[player][i].ym = -5;
	  playsound(SND_EVILBILL_DIE, 1, 1);
	  
	  dancing[player] = 1;
	}
      else if (aliens[player][i].shields >= 8 && aliens[player][i].shields <= 10)
	{
	  /* Shields low: */
	  
	  playsound(SND_EVILBILL_LOW, 1, 0);
	}
      else
	{
	  /* "Ow!" */
	  
	  playsound(SND_EVILBILL_HURT, 1, 0);
	}
    }
}


/* Add a bullet: */

void add_bullet(int xx, int yy, int owner)
{
  int i, found;
  
  
  /* Find a slot: */
  
  found = -1;
  
  for (i = 0; i < MAX_BULLETS && found == -1; i++)
    {
      if (bullets[i].alive == 0)
	found = i;
    }
  
  
  /* Turn the bullet on: */
  
  if (found != -1)
    { 
      bullets[found].alive = 1;
      bullets[found].timer = 50;
      bullets[found].owner = owner;
      
      bullets[found].x = xx;
      bullets[found].y = yy;
      
      bullets[found].xm = (x[player] - xx) / 40;
      bullets[found].ym = (y[player] - yy) / 40;

      playsound(SND_BULLET, 1, 0);
    }
}


/* Set the current message: */

void set_message(int img)
{
  mesg_img = img;
  mesg_timer = 50;
}


/* Kill the player: */

void kill_player(void)
{
  int i;
  
  
  /* Make the player die, and the screen flash and some noise: */
  
  dying[player] = 1;
  flash = 1;
  playsound(SND_DIE1 + (rand() % 2), 3, 1);
  
  
  /* Kill any penguins we were carrying: */
  
  for (i = 0; i < MAX_PENGUINOIDS; i++)
    {
      if (penguinoids[player][i].alive == 1)
	{
	  if (penguinoids[player][i].mode == PENG_MODE_SAVED)
	    penguinoids[player][i].alive = 0;
	}
    }
}


/* Add some score: */

void add_score(int add)
{
  score[player] = score[player] + add;
  
  if ((score[player] % 10000) < ((score[player] - add) % 10000))
    {
      lives[player]++;
      bombs[player]++;
      
      oneup_effect = 32;
      oneup_effect_counter = 3;
      
      playsound(SND_ONEUP, 1, 1);
    }
}


/* Detonate a smart bomb: */

void smartbomb(void)
{
  int i, tmp_x;
  
  if (bombs[player] > 0)
    {
      /* Remove a bomb: */
      
      bombs[player]--;
      
      
      /* Kill all visible aliens! */
      
      for (i = 0; i < MAX_ALIENS; i++)
	{
	  if (aliens[player][i].alive == 1)
	    {
	      /* Relative position? */
	      
	      tmp_x = aliens[player][i].x - (x[player] -
				     scroll[player]);

	      while (tmp_x < 0)
		tmp_x = tmp_x + (LAND_WIDTH * 32);
	      
	      while (tmp_x >= (LAND_WIDTH * 32))
		tmp_x = tmp_x - (LAND_WIDTH * 32);
	      
	      
	      /* Is it visible (on screen)? */
	      
	      if (tmp_x >= -32 && tmp_x <= 672)
		{
		  /* Kill the alien: */
		  
		  kill_alien(i);
		}
	    }
	}
      
      
      /* Kill all visible bullets! */
      
      for (i = 0; i < MAX_BULLETS; i++)
	{
	  if (bullets[i].alive == 1)
	    {
	      /* Relative position? */
	      
	      tmp_x = bullets[i].x - (x[player] -
				      scroll[player]);
	      
	      while (tmp_x >= (LAND_WIDTH * 32))
		tmp_x = tmp_x - (LAND_WIDTH * 32);
	      
	      
	      /* Is it visible (on screen)? */
	      
	      if (tmp_x >= -64 && tmp_x <= 672)
		{
		  /* Kill the alien: */
		  
		  bullets[i].alive = 0;
		}
	    }
	}
      
      
      /* Noise and flash: */
      
      playsound(SND_SMARTBOMB, 1, 1);
      
      flash = 2;
    }
}


/* Pause screen: */

void pause_screen(void)
{
  SDL_Event event;
  SDL_Surface * old_screen;
  int done, frame, x, y, xm, ym;
  Uint32 last_time;
  SDL_Rect src, dest;
  
  
  /* Create backup of current display: */
	sceDisplayWaitVblankStart(); 
  SDL_Flip(screen);
  old_screen = SDL_DisplayFormat(screen);

  
  /* Stop playing music: */

//#ifdef SOUND
  if (use_sound == 1)
    Mix_PauseMusic();
//#endif /* #ifdef SOUND */

  
  /* --- Pause screen loop: --- */

  done = 0;
  frame = 0;
  
  x = (rand() % (320 - 16));
  y = (rand() % (240 - 16));
  xm = (rand() % 10) - 5;
  ym = (rand() % 10) - 5;
  
  SDL_Delay(100);
  while (SDL_PollEvent(&event))
	SDL_Delay(20);
  do
    {
      last_time = SDL_GetTicks();
      
      
      /* Check for keypresses: */
      
      while (SDL_PollEvent(&event))
	{
	  if (event.type == SDL_KEYDOWN)
	    done = 1;
	  
//#ifdef JOY_YES
	  if (event.type == SDL_JOYBUTTONDOWN &&
	      (num_joysticks < 2 || event.jbutton.which == player))
	    {
	      done = 1;
	    }
//#endif /* #ifdef JOY_YES */
	}
      
      
      /* Draw screen: */
      
      frame++;
      
      
	  for (y = (frame % 2); y < 240; y = y + 2)
	    {
	      dest.x = (my_cos[(y * 2) % 16] * (my_cos[frame % 16] * 5));
	      dest.y = y + (my_cos[(y + frame) % 16]);
	      dest.w = 320;
	      dest.h = 1;
	      
	      src.x = 0;
	      src.y = y;
	      src.w = 320;
	      src.h = 1;
	      
	      SDL_BlitSurface(old_screen, &src, screen, &dest);
	    }
	if ((frame%20)>10)
      		write_centered_text(24,"PAUSE");
sceDisplayWaitVblankStart(); 
      SDL_Flip(screen);
      
      
      /* Pause til next frame: */
      
      if (SDL_GetTicks() < last_time + 60)
	SDL_Delay(last_time + 60 - SDL_GetTicks());
    }
  while (!done);

  SDL_FreeSurface(old_screen);


  /* Continue playing music: */

//#ifdef SOUND
  if (use_sound == 1)
    {
      Mix_ResumeMusic();
      Mix_VolumeMusic(vol_music * (MIX_MAX_VOLUME / 5));
    }
//#endif /* #ifdef SOUND */
}


/* Write text on the option screen: */

void write_text(int x, int y, char * str)
{
  int i, c;
  SDL_Rect src, dest;
  
  for (i = 0; i < strlen(str); i++)
    {
      c = -1;
      
      if (str[i] >= '0' && str[i] <= '9')
	c = str[i] - '0';
      else if (str[i] >= 'A' && str[i] <= 'Z')
	c = str[i] - 'A' + 10;
      else if (str[i] >= 'a' && str[i] <= 'z')
	c = str[i] - 'a' + 36;
      else if (str[i] == '#')
	c = 62;
      else if (str[i] == '=')
	c = 63;
      else if (str[i] == '_')
	c = -2;
      else if (str[i] == '-')
	c = -3;
      
      if (c >= 0)
	{
	  src.x = c * 8;
	  src.y = 0;
	  src.w = 8;
	  src.h = 8;
	  
	  dest.x = (x + i) * 8;
	  dest.y = y * 10;
	  dest.w = 8;
	  dest.h = 8;
	  
	  SDL_BlitSurface(images[IMG_OPTION_TEXT], &src,
			  screen, &dest);
	}
      else if (c == -2 || c == -3)
	{
	  dest.x = (x + i) * 8;
	  
	  if (c == -2)
	    dest.y = y * 10 + 7;
	  else if (c == -3)
	    dest.y = y * 10 + 3;
	  
	  dest.w = 8;
	  dest.h = 1;
	  
	  SDL_FillRect(screen, &dest,
		       SDL_MapRGB(screen->format, 0xCC, 0xCC, 0xCC));
	}
    }
  
//  SDL_UpdateRect(screen, x * 8, y * 10, strlen(str) * 8, 8);
}


/* Write text, inverted: */

void write_text_inv(int x, int y, char * str)
{
  SDL_Rect dest;
  
  write_text(x, y, str);
  
  dest.x = x * 8;
  dest.y = y * 10 - 1;
  dest.w = strlen(str) * 8;
  dest.h = 1;
  
  SDL_FillRect(screen, &dest, SDL_MapRGB(screen->format, 128, 128, 128));
  
  dest.x = x * 8;
  dest.y = y * 10 + 8;
  dest.w = strlen(str) * 8;
  dest.h = 1;
  
//  SDL_FillRect(screen, &dest, SDL_MapRGB(screen->format, 128, 128, 128));
}


/* Write text, horizontally centered... */

void write_centered_text(int y, char * str)
{
  write_text(20 - (strlen(str) / 2), y/2, str);
}


/* Write numbers on the option screen: */

void write_num(int x, int y, int v)
{
  char str[24];
  
  sprintf(str, "%d", v);
  write_text(x, y, str);
}


/* Return the value of a particular line's option: */

int option_value(int opt_line)
{
  if (opt_line == 3)
    return (vol_effects);
  else if (opt_line == 4)
    return (vol_music);
  else
    return (0);
}
  

