// Time Travel
// Copyright (C) 1999 by Dan Lynch

// A "Mimas Entertainment" production

// include header files
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <allegro.h>
#include "time.h"

// heights of the map sprites
const int HEIGHT[] = {50, 27, 32, 29, 19, 68, 85};

// variables for timers
volatile int speed = 0;
volatile int move = 0;

int frame;

/*****************************************************************************

		STATUS STRUCT

*****************************************************************************/

struct STATUS
{	int time;
	double man_x, man_y;
   int location;
   int direction;

   int deer_asked;
   int deer_inspected;
   int deer_reported;

   int apple_traded;
   int stone_found;

   int cave_inspected;
   int giant_killed;
};

/*****************************************************************************

		FUNCTION PROTOTYPES

*****************************************************************************/

char menu(void);
int speed_test(void);
void title_screen(void);
void new_game(void);
void old_game(void);
void instructions(void);
void credits(void);
void game(STATUS);
void new_area(STATUS*);
void player_move(STATUS*);
void save_game(STATUS);
void text_box(int, int);
void goodbye_screen(void);

/*****************************************************************************

		TIMER FUNCTIONS

*****************************************************************************/

void speed_timer(...)
{	speed++;
}
END_OF_FUNCTION(speed_timer);

void move_timer(...)
{	move++;
}
END_OF_FUNCTION(move_timer);

int found = FALSE;
int lost_x = -1;
int lost_y = -1;

/*****************************************************************************

		CHECK FUNCTION

*****************************************************************************/

void check(BITMAP* bmp, int x, int y, int d)
{	int colour = getpixel(bmp, x + 6, y + 25);

	if (colour > 0 && colour != 14 && colour != 11 && !found)
	{	found = TRUE;
   	lost_x = x;
      lost_y = y;
   }
}

/*****************************************************************************

		MIDI_FADE FUNCTION

*****************************************************************************/

void midi_fade(MIDI* midi)
{  stop_midi();
   play_looped_midi(midi, 0, -1);
}

// declare variable needed for datafile
DATAFILE *data;

/*****************************************************************************

		MAIN FUNCTION

*****************************************************************************/

int main(int argc, char *argv[])
{	// declare variable needed for datafile
   char buf[80];

   // set up Allegro
   allegro_init();
	install_timer();
   install_mouse();
	install_keyboard();

	// seed randomizer
   time_t t;
   srand((unsigned) time(&t));

   // load datafile into memory
   strcpy(buf, argv[0]);
   strcpy(get_filename(buf), "time.dat");
   data = load_datafile(buf);
   if (!data) // make sure datafile is loaded
   {  allegro_exit();
      printf("Error loading time.dat!\n\n");
      getch();
      return 1;
   }

   // set up graphics mode
   set_color_depth(16);
	set_gfx_mode(GFX_AUTODETECT, 800, 600, 0, 0);

   // set up sound
   install_sound(DIGI_AUTODETECT, MIDI_AUTODETECT, NULL);
   set_volume(-1, 255);

   if (speed_test() > 2400)
   	text_box(3, 19);

	char choice = 1;

   while (choice)
   {  if (choice == 1)
      	title_screen();
      else if (choice == 2)
      	new_game();
      else if (choice == 3)
      	old_game();
      else if (choice == 4)
      	instructions();
      else if (choice == 5)
      	credits();

     	choice = menu();
   }
}

/*****************************************************************************

		SPEED_TEST FUNCTION

*****************************************************************************/

int speed_test()
{  BITMAP* buffer = create_bitmap(800, 600);

   set_color_depth(8);
   BITMAP* big = create_bitmap(800, 600);
   BITMAP* small = create_bitmap(25, 25);
   set_color_depth(16);

   LOCK_VARIABLE(speed);
   LOCK_FUNCTION(speed_timer);

   install_int(speed_timer, 5);

   set_palette(black_palette);
   for (int i = 0; i < 25; i++)
   {  blit (big, buffer, 0, 0, 0, 0, 800, 600);

      for (int j = 0; j < 100; j++)
   		draw_sprite(buffer, small, rand() % 775, rand() % 575);

      blit (buffer, screen, 0, 0, 0, 0, 800, 600);
   }
   clear(screen);
   return speed * 5;
}

/*****************************************************************************

		MENU FUNCTION

*****************************************************************************/

char menu(void)
{  clear(screen);

	show_mouse(NULL);

	set_palette((PALETTE)data[PAINT_PAL].dat);
	blit((BITMAP*)data[MENU_BMP].dat, screen, 0, 0, 0, 0, 800, 600);

   midi_fade((MIDI*)data[MENU_MIDI].dat);

	show_mouse(screen);

   while(1)
   {	if (mouse_x >= 327 && mouse_x <= 472 && mouse_y >= 188 && mouse_y <= 226 && mouse_b)
   		return 1;
   	else if (mouse_x >= 226 && mouse_x <= 370 && mouse_y >= 326 && mouse_y <= 364 && mouse_b)
			return 2;
   	else if (mouse_x >= 417 && mouse_x <= 562 && mouse_y >= 326 && mouse_y <= 364 && mouse_b)
			return 3;
   	else if (mouse_x >= 330 && mouse_x <= 474 && mouse_y >= 259 && mouse_y <= 297 && mouse_b)
			return 4;
   	else if (mouse_x >= 334 && mouse_x <= 478 && mouse_y >= 402 && mouse_y <= 440 && mouse_b)
			return 5;
   	else if (mouse_x >= 334 && mouse_x <= 478 && mouse_y >= 478 && mouse_y <= 516 && mouse_b)
			return 0;
   }
}

/*****************************************************************************

		TITLE_SCREEN FUNCTION

*****************************************************************************/

void title_screen(void)
{  set_palette((PALETTE)data[PAINT_PAL].dat);
	show_mouse(NULL);
	blit((BITMAP*)data[TITLE_BMP].dat, screen, 0, 0, 0, 0, 800, 600);
   rest(1000);
	while (!keypressed() && !mouse_b);
}

/*****************************************************************************

		NEW_GAME FUNCTION

*****************************************************************************/

void new_game(void)
{  STATUS start;

	start.time = 0;
	start.man_x = 122;
   start.man_y = 426;
   start.location = 0;
   start.direction = 3;

   start.deer_asked = FALSE;
   start.deer_inspected = FALSE;
   start.deer_reported = FALSE;

   start.apple_traded = FALSE;
   start.stone_found = FALSE;

   start.cave_inspected = FALSE;
   start.giant_killed = FALSE;

	show_mouse(NULL);
   clear(screen);

   text_box(3, 41);

	game(start);
}

/*****************************************************************************

		OLD_GAME FUNCTION

*****************************************************************************/

void old_game(void)
{  STATUS old;
	FILE* savefile = fopen("time.sav", "rb");
	fread(&old, sizeof(STATUS), 1, savefile);
   fclose(savefile);

	show_mouse(NULL);
   game(old);
}

/*****************************************************************************

		INSTRUCITONS FUNCTION

*****************************************************************************/

void instructions(void)
{	show_mouse(NULL);
	clear(screen);
	text_box(0, 52);
}

/*****************************************************************************

		CREDITS FUNCTION

*****************************************************************************/

void credits(void)
{	show_mouse(NULL);
	clear(screen);
	text_box(0, 53);
}

/*****************************************************************************

		GAME FUNCTION

*****************************************************************************/

void game(STATUS status)
{	clear(screen);

   // double buffering
	BITMAP* buffer = create_bitmap(800, 600);

   // for object display
	int obj_qty, obj_x, obj_y, obj_y_old, obj_type;

   int win = FALSE;
   int dead = FALSE;

   stop_midi();

   frame = 0;

   // for frame-dropping
   LOCK_VARIABLE(move);
	LOCK_FUNCTION(move_timer);
   install_int(move_timer, 40);

   while (!key[KEY_ESC] && !win && !dead)
   {  while (move < 1); // speed limiting
      move--;

      if (move < 1) // frame dropping
      {	set_palette((PALETTE)data[PAINT_PAL].dat);
	   	blit((BITMAP*)data[MAP0_BMP + status.time].dat, buffer, 0, 0, 0, 0, 800, 600);

	   	obj_y_old = -100;
   		sscanf((char*)data[OBJECT0_TXT + status.time].dat, "%d", &obj_qty);
   		for (int i = 0; i < obj_qty; i++)
			{	sscanf((char*)data[OBJECT0_TXT + status.time].dat + 5 + i * 11, "%d %d %d",
					&obj_x, &obj_y, &obj_type);

         	if (status.man_y + 25 >= obj_y_old && status.man_y + 25 <= obj_y)
            {	if (status.direction == 0 && frame % 6 < 3)
						draw_sprite(buffer, (BITMAP*)data[MAN000_BMP + status.time].dat, (int) status.man_x,
							(int) status.man_y);
               else if (status.direction == 0)
						draw_sprite(buffer, (BITMAP*)data[MAN010_BMP + status.time].dat, (int) status.man_x,
							(int) status.man_y);
            	else if (status.direction == 1 && frame % 6 < 3)
						draw_sprite(buffer, (BITMAP*)data[MAN100_BMP + status.time].dat, (int) status.man_x,
							(int) status.man_y);
            	else if (status.direction == 1)
						draw_sprite(buffer, (BITMAP*)data[MAN110_BMP + status.time].dat, (int) status.man_x,
							(int) status.man_y);
            	else if (status.direction == 2 && frame % 6 < 3)
						draw_sprite(buffer, (BITMAP*)data[MAN200_BMP + status.time].dat, (int) status.man_x,
							(int) status.man_y);
            	else if (status.direction == 2)
						draw_sprite(buffer, (BITMAP*)data[MAN210_BMP + status.time].dat, (int) status.man_x,
							(int) status.man_y);
            	else if (status.direction == 3 && frame % 6 < 3)
						draw_sprite(buffer, (BITMAP*)data[MAN300_BMP + status.time].dat, (int) status.man_x,
							(int) status.man_y);
            	else if (status.direction == 3)
						draw_sprite(buffer, (BITMAP*)data[MAN310_BMP + status.time].dat, (int) status.man_x,
							(int) status.man_y);
            }

         	obj_y_old = obj_y;
				draw_sprite(buffer, (BITMAP*)data[SPRITE0_BMP + obj_type].dat, (int)
					obj_x, (int) (obj_y - HEIGHT[obj_type]));
   		}

      	if (status.man_y + 25 >= obj_y)
			{	if (status.direction == 0 && frame % 6 < 3)
					draw_sprite(buffer, (BITMAP*)data[MAN000_BMP + status.time].dat, (int) status.man_x,
						(int) status.man_y);
            else if (status.direction == 0)
					draw_sprite(buffer, (BITMAP*)data[MAN010_BMP + status.time].dat, (int) status.man_x,
						(int) status.man_y);
            else if (status.direction == 1 && frame % 6 < 3)
					draw_sprite(buffer, (BITMAP*)data[MAN100_BMP + status.time].dat, (int) status.man_x,
						(int) status.man_y);
            else if (status.direction == 1)
					draw_sprite(buffer, (BITMAP*)data[MAN110_BMP + status.time].dat, (int) status.man_x,
						(int) status.man_y);
            else if (status.direction == 2 && frame % 6 < 3)
					draw_sprite(buffer, (BITMAP*)data[MAN200_BMP + status.time].dat, (int) status.man_x,
						(int) status.man_y);
            else if (status.direction == 2)
					draw_sprite(buffer, (BITMAP*)data[MAN210_BMP + status.time].dat, (int) status.man_x,
						(int) status.man_y);
            else if (status.direction == 3 && frame % 6 < 3)
					draw_sprite(buffer, (BITMAP*)data[MAN300_BMP + status.time].dat, (int) status.man_x,
						(int) status.man_y);
            else if (status.direction == 3)
					draw_sprite(buffer, (BITMAP*)data[MAN310_BMP + status.time].dat, (int) status.man_x,
						(int) status.man_y);
         }

         /*
         text_mode(0);
         textprintf(buffer, font, 20, 20, 65535, "%d %d", (int)status.man_x, (int)status.man_y);
         text_mode(-1);
         */

         blit(buffer, screen, 0, 0, 0, 0, 800, 600);
      }

      new_area(&status);

      if ((status.location == 9 || status.location == 10) && status.time == 0)
      	dead = TRUE;

		else if (getpixel((BITMAP*)data[WALK0_BMP].dat, (int)status.man_x + 6,
				(int)status.man_y + 25) == 6 && status.time == 0)
      {	// hole in mound
      	if (!status.giant_killed)
      		text_box(status.time, 17);
         else
      		text_box(status.time, 51);

         status.cave_inspected = TRUE;
         status.man_x = 168;
         status.man_y = 85;
      }
		else if (getpixel((BITMAP*)data[WALK0_BMP].dat, (int)status.man_x + 6,
				(int)status.man_y + 25) == 13 && status.time == 0)
      {	// elder's cave
      	text_box(status.time, 18);
         status.man_x = 605;
         status.man_y = 440;
		}
		else if (getpixel((BITMAP*)data[WALK0_BMP].dat, (int)status.man_x + 6,
				(int)status.man_y + 25) == 2 && status.time == 0)
      {	// family tent
      	text_box(status.time, 20);
         status.man_x = 266;
         status.man_y = 388;
		}
		else if (getpixel((BITMAP*)data[WALK0_BMP].dat, (int)status.man_x + 6,
				(int)status.man_y + 25) == 185 && status.time == 0)
      {	// youth tent

         if (!status.deer_reported)
      		text_box(status.time, 21);
         else if (status.deer_reported && !status.apple_traded && !status.stone_found)
         {	text_box(status.time, 34);
         	status.apple_traded = TRUE;
         }
         else if (status.deer_reported && !status.apple_traded)
         {	text_box(status.time, 49);
         	status.apple_traded = TRUE;
         }
         else
         	text_box(status.time, 35);

         status.man_x = 351;
         status.man_y = 384;
		}
		else if (getpixel((BITMAP*)data[WALK0_BMP].dat, (int)status.man_x + 6,
				(int)status.man_y + 25) == 174 && status.time == 0)
      {	// herder tent

         if (!status.deer_inspected)
      	{	text_box(status.time, 22);
         	status.deer_asked = TRUE;
         }
         else if (status.deer_inspected && !status.deer_reported)
         {	text_box(status.time, 32);
         	status.deer_reported = TRUE;
         }
         else
         	text_box(status.time, 33);

         status.man_x = 262;
         status.man_y = 487;
		}
		else if (getpixel((BITMAP*)data[WALK0_BMP].dat, (int)status.man_x + 6,
				(int)status.man_y + 25) == 98 && status.time == 0)
      {	// chief tent
      	text_box(status.time, 23);
         status.man_x = 382;
         status.man_y = 477;
		}
		else if (getpixel((BITMAP*)data[WALK0_BMP].dat, (int)status.man_x + 6,
				(int)status.man_y + 25) == 17 && status.time == 0)
      {	// big cave
      	if (status.cave_inspected)
      		text_box(status.time, 40);
         else
      		text_box(status.time, 39);

         status.location = 0;
         status.time++;
		}

      else if (getpixel((BITMAP*)data[WALK1_BMP].dat, (int)status.man_x + 6,
				(int)status.man_y + 25) == 9 && status.time == 1)
      {	// statue
      	text_box(status.time, 45);
         status.time++;
         status.location = 0;
		}

      else if (getpixel((BITMAP*)data[WALK2_BMP].dat, (int)status.man_x + 6,
				(int)status.man_y + 25) == 14 && status.time == 2)
      {	// GPF
      	text_box(status.time, 50);
         dead = TRUE;
		}
      else if (getpixel((BITMAP*)data[WALK2_BMP].dat, (int)status.man_x + 6,
				(int)status.man_y + 25) == 9 && status.time == 2)
      {	// upper left
      	text_box(status.time, 47);
         text_box(status.time, 48);
         win = TRUE;
		}

      if (status.location == 14 && rand() % 1001 > 996 && status.time == 0)
      {	// lost in forest
      	text_box(status.time, 14);

         found = FALSE;
         double theta = random() % 360;
      	do_line((BITMAP*)data[WALK0_BMP].dat, (int)status.man_x, (int)status.man_y, (int)(status.man_x + 1000 * cos(theta)), (int)(status.man_y + 1000 * sin(theta)), NULL, check);

         if (!found)
         {	text_box(status.time, 28);
         	dead = TRUE;
         }
         else
         {	status.man_x = lost_x;
         	status.man_y = lost_y;
         }
		}
      else if (status.location == 14 && rand() % 2000 < 1 && status.time == 0 && !status.stone_found)
		{	// found stone
      	text_box(status.time, 36);
			status.stone_found = TRUE;
      }

      player_move(&status);

   }

   if (dead)
   {	midi_fade((MIDI*)data[DEAD_MIDI].dat);
   	text_box(status.time, 15);
   }
   else if (win)
   {	midi_fade((MIDI*)data[WIN_MIDI].dat);
   	text_box(status.time, 16);
   }
}

/*****************************************************************************

		NEW_AREA FUNCTION

*****************************************************************************/

void new_area(STATUS* status)
{	if ((*status).location == 0 && (*status).time == 0)
   {  // starting
   	midi_fade((MIDI*)data[FOREST_MIDI].dat);
      text_box((*status).time, 42);
    	(*status).location = 137;
   }
 	else if ((*status).location == 0 && (*status).time == 1)
   {  // medieval
   	midi_fade((MIDI*)data[FOREST_MIDI].dat);
      text_box((*status).time, 43);
    	(*status).location = 3;
   }
 	else if ((*status).location == 0 && (*status).time == 2)
   {  // future
   	midi_fade((MIDI*)data[FOREST_MIDI].dat);
      text_box((*status).time, 46);
    	(*status).location = 3;
   }

	else if ((*status).location == 3 && getpixel((BITMAP*)data[WALK0_BMP + (*status).time].dat, (int)(*status).man_x
			+ 6, (int)(*status).man_y + 25) == 14 && (*status).time == 0)
   {  // normal to forest
   	midi_fade((MIDI*)data[FOREST_MIDI].dat);
   	text_box((*status).time, 0);
    	(*status).location = 14;
   }
	else if ((*status).location == 3 && getpixel((BITMAP*)data[WALK0_BMP + (*status).time].dat, (int)
			(*status).man_x + 6, (int)(*status).man_y + 25) == 5 && (*status).time == 0)
   {  // normal to path
   	midi_fade((MIDI*)data[FOREST_MIDI].dat);
	  	text_box((*status).time, 1);
     	(*status).location = 5;
   }
	else if ((*status).location == 3 && getpixel((BITMAP*)data[WALK0_BMP + (*status).time].dat, (int)
			(*status).man_x + 6, (int)(*status).man_y + 25) == 137 && (*status).time == 0)
   {  // normal to ring
	  	text_box((*status).time, 24);
     	(*status).location = 137;
   }
	else if ((*status).location == 3 && getpixel((BITMAP*)data[WALK0_BMP + (*status).time].dat, (int)
			(*status).man_x + 6, (int)(*status).man_y + 25) == 7 && (*status).time == 0)
   {  // normal to deerfield
   	midi_fade((MIDI*)data[DEER_MIDI].dat);

      if ((*status).deer_asked && !(*status).deer_inspected)
   	{	text_box((*status).time, 29);
         (*status).deer_inspected = TRUE;
      }
      else
      	text_box((*status).time, 31);

   	(*status).location = 7;
   }
	else if ((*status).location == 3 && getpixel((BITMAP*)data[WALK0_BMP + (*status).time].dat, (int)
			(*status).man_x + 6, (int)(*status).man_y + 25) == 8 && (*status).time == 0)
   {  // normal to village
   	midi_fade((MIDI*)data[VILLAGE_MIDI].dat);
     	(*status).location = 8;
   }

	else if ((*status).location == 14 && getpixel((BITMAP*)data[WALK0_BMP + (*status).time].dat,
			(int)(*status).man_x + 6, (int)(*status).man_y + 25) == 3 && (*status).time == 0)
   {  // forest to normal
   	midi_fade((MIDI*)data[GRASS_MIDI].dat);
     	text_box((*status).time, 2);
      (*status).location = 3;
   }
	else if ((*status).location == 14 && getpixel((BITMAP*)data[WALK0_BMP + (*status).time].dat,
			(int)(*status).man_x + 6, (int)(*status).man_y + 25) == 5 && (*status).time == 0)
   {  // forest to path
     	text_box((*status).time, 3);
      (*status).location = 5;
   }
	else if ((*status).location == 14 && getpixel((BITMAP*)data[WALK0_BMP + (*status).time].dat,
			(int)(*status).man_x + 6, (int)(*status).man_y + 25) == 9 && (*status).time == 0)
   {	// forest to sling
   	if (!(*status).giant_killed)
		{	midi_fade((MIDI*)data[SCARY_MIDI].dat);
  			text_box((*status).time, 4);

     		if ((*status).apple_traded && (*status).stone_found)
     		{	text_box((*status).time, 37);
     			(*status).giant_killed = TRUE;
				(*status).location = 5;
    		}
      	else
      	{	text_box((*status).time, 38);
    			(*status).location = 9;
      	}

   	}
      else
      	(*status).location = 5;

   }
	else if ((*status).location == 14 && getpixel((BITMAP*)data[WALK0_BMP + (*status).time].dat,
			(int)(*status).man_x + 6, (int)(*status).man_y + 25) == 12 && (*status).time == 0)
   {  // forest to behind
   	midi_fade((MIDI*)data[GRASS_MIDI].dat);
     	text_box((*status).time, 5);
      (*status).location = 12;
   }

	else if ((*status).location == 5 && getpixel((BITMAP*)data[WALK0_BMP + (*status).time].dat,
			(int)(*status).man_x + 6, (int)(*status).man_y + 25) == 3 && (*status).time == 0)
   {  // path to normal
   	midi_fade((MIDI*)data[GRASS_MIDI].dat);
   	text_box((*status).time, 6);
      (*status).location = 3;
   }
	else if ((*status).location == 5 && getpixel((BITMAP*)data[WALK0_BMP + (*status).time].dat,
			(int)(*status).man_x + 6, (int)(*status).man_y + 25) == 14 && (*status).time == 0)
   {  // path to forest
    	text_box((*status).time, 7);
      (*status).location = 14;
   }
	else if ((*status).location == 5 && getpixel((BITMAP*)data[WALK0_BMP + (*status).time].dat, (int)
			(*status).man_x + 6, (int)(*status).man_y + 25) == 9 && (*status).time == 0)
   {  // path to sling
      if (!(*status).giant_killed)
		{	midi_fade((MIDI*)data[SCARY_MIDI].dat);
  			text_box((*status).time, 8);

     		if ((*status).apple_traded && (*status).stone_found)
     		{	text_box((*status).time, 37);
     			(*status).giant_killed = TRUE;
				(*status).location = 5;
     		}
      	else
      	{	text_box((*status).time, 38);
   			(*status).location = 9;
      	}
		}
      else
      	(*status).location = 5;
   }

	else if ((*status).location == 12 && getpixel((BITMAP*)data[WALK0_BMP + (*status).time].dat, (int)(*status).man_x + 6, (int)(*status).man_y + 25) == 14 && (*status).time == 0)
   {  // behind to forest
   	midi_fade((MIDI*)data[FOREST_MIDI].dat);
     	text_box((*status).time, 12);
      (*status).location = 14;
   }
	else if ((*status).location == 12 && getpixel((BITMAP*)data[WALK0_BMP + (*status).time].dat,
			(int)(*status).man_x + 6, (int)(*status).man_y + 25) == 9 && (*status).time == 0)
   {  // behind to sling
      if (!(*status).giant_killed)
   	{	midi_fade((MIDI*)data[SCARY_MIDI].dat);
  			text_box((*status).time, 13);

     		if ((*status).apple_traded && (*status).stone_found)
     		{	text_box((*status).time, 37);
     			(*status).giant_killed = TRUE;
				(*status).location = 5;
     		}
      	else
      	{	text_box((*status).time, 38);
   			(*status).location = 9;
      	}
      }
      else
      	(*status).location = 5;
   }

   else if ((*status).location == 137 && getpixel((BITMAP*)data[WALK0_BMP + (*status).time].dat,
			(int)(*status).man_x + 6, (int)(*status).man_y + 25) == 3 && (*status).time == 0)
   {  // ring to normal
   	midi_fade((MIDI*)data[GRASS_MIDI].dat);
   	text_box((*status).time, 25);
      (*status).location = 3;
   }

   else if ((*status).location == 80 && getpixel((BITMAP*)data[WALK0_BMP + (*status).time].dat,
			(int)(*status).man_x + 6, (int)(*status).man_y + 25) == 3 && (*status).time == 0)
   {  // fireside to normal
      (*status).location = 3;
   }
   else if ((*status).location == 80 && getpixel((BITMAP*)data[WALK0_BMP + (*status).time].dat,
			(int)(*status).man_x + 6, (int)(*status).man_y + 25) == 10 && (*status).time == 0)
   {  // fireside to fire
   	text_box((*status).time, 27);
      (*status).location = 10;
   }

   else if ((*status).location == 7 && getpixel((BITMAP*)data[WALK0_BMP + (*status).time].dat,
			(int)(*status).man_x + 6, (int)(*status).man_y + 25) == 3 && (*status).time == 0)
   {  // deerfield to normal
   	midi_fade((MIDI*)data[GRASS_MIDI].dat);
      (*status).location = 3;
   }

	else if ((*status).location == 8 && getpixel((BITMAP*)data[WALK0_BMP + (*status).time].dat, (int)
			(*status).man_x + 6, (int)(*status).man_y + 25) == 3 && (*status).time == 0)
   {  // village to normal
   	midi_fade((MIDI*)data[GRASS_MIDI].dat);
     	(*status).location = 3;
   }
	else if ((*status).location == 8 && getpixel((BITMAP*)data[WALK0_BMP + (*status).time].dat, (int)
			(*status).man_x + 6, (int)(*status).man_y + 25) == 80 && (*status).time == 0)
   {  // village to fireside
	  	text_box((*status).time, 26);
     	(*status).location = 80;
   }

	else if ((*status).location == 3 && getpixel((BITMAP*)data[WALK0_BMP + (*status).time].dat, (int)(*status).man_x
			+ 6, (int)(*status).man_y + 25) == 12 && (*status).time == 1)
   {  // normal to strike
   	midi_fade((MIDI*)data[SCARY_MIDI].dat);
   	text_box((*status).time, 44);
    	(*status).location = 12;
   }
	else if ((*status).location == 12 && getpixel((BITMAP*)data[WALK0_BMP + (*status).time].dat, (int)(*status).man_x
			+ 6, (int)(*status).man_y + 25) == 3 && (*status).time == 1)
   {  // strike to normal
   	midi_fade((MIDI*)data[GRASS_MIDI].dat);
    	(*status).location = 3;
   }

   /*
   BITMAP *bmp;
   PALETTE pal;
   bmp = create_sub_bitmap(screen, 0, 0, SCREEN_W, SCREEN_H);
   save_bitmap("dump.pcx", bmp, pal);
   destroy_bitmap(bmp);
   */
}

/*****************************************************************************

		PLAYER_MOVE FUNCTION

*****************************************************************************/

void player_move(STATUS *status)
{	if (((key[KEY_RIGHT] && key[KEY_DOWN]) || key[KEY_PGDN]) &&
			getpixel((BITMAP*)data[WALK0_BMP + (*status).time].dat, (int)((*status).man_x + 6 +
			0.7071), (int)((*status).man_y + 25 + 0.7071)))
   {	(*status).man_x += 0.7071; // faster than sqrt(0.5)
      (*status).man_y += 0.7071;
      (*status).direction = 0;
   }
	else if (((key[KEY_RIGHT] && key[KEY_UP]) || key[KEY_PGUP]) &&
			getpixel((BITMAP*)data[WALK0_BMP + (*status).time].dat, (int)((*status).man_x + 6 +
			0.7071), (int)((*status).man_y + 25 - 0.7071)))
   {	(*status).man_x += 0.7071; // faster than sqrt(0.5)
      (*status).man_y -= 0.7071;
      (*status).direction = 0;
   }
	else if (((key[KEY_LEFT] && key[KEY_DOWN]) || key[KEY_END]) &&
			getpixel((BITMAP*)data[WALK0_BMP + (*status).time].dat, (int)((*status).man_x + 6 -
			0.7071), (int)((*status).man_y + 25 + 0.7071)))
   {	(*status).man_x -= 0.7071; // faster than sqrt(0.5)
      (*status).man_y += 0.7071;
      (*status).direction = 2;
   }
	else if (((key[KEY_LEFT] && key[KEY_UP]) || key[KEY_HOME]) &&
			getpixel((BITMAP*)data[WALK0_BMP + (*status).time].dat, (int)((*status).man_x + 6 -
			0.7071), (int)((*status).man_y + 25 - 0.7071)))
   {	(*status).man_x -= 0.7071; // faster than sqrt(0.5)
      (*status).man_y -= 0.7071;
   }

	else if ((key[KEY_RIGHT]) && getpixel((BITMAP*)data[WALK0_BMP + (*status).time].dat,
			(int)((*status).man_x + 6 + 1), (int)((*status).man_y + 25)))
   {	(*status).man_x += 1.0;
      (*status).direction = 0;
   }
	else if ((key[KEY_LEFT]) && getpixel((BITMAP*)data[WALK0_BMP + (*status).time].dat,
			(int)((*status).man_x + 6 - 1), (int)((*status).man_y + 25)))
   {	(*status).man_x -= 1.0;
      (*status).direction = 2;
   }
	else if ((key[KEY_DOWN]) && getpixel((BITMAP*)data[WALK0_BMP + (*status).time].dat,
			(int)((*status).man_x + 6), (int)((*status).man_y + 25 + 1)))
   {	(*status).man_y += 1.0;
      (*status).direction = 3;
   }
	else if ((key[KEY_UP]) && getpixel((BITMAP*)data[WALK0_BMP + (*status).time].dat, (int)
			((*status).man_x + 6), (int)((*status).man_y + 25 - 1)))
   {	(*status).man_y -= 1.0;
      (*status).direction = 1;
   }
   else if (key[KEY_F5])
   	save_game(*status);
   else
   	frame--;

   frame++;
}


/*****************************************************************************

		SAVE_GAME FUNCTION

*****************************************************************************/

void save_game(STATUS status)
{	FILE* savefile = fopen("time.sav", "wb");
	fwrite(&status, sizeof(STATUS), 1, savefile);
   text_box(status.time, 30);
   fclose(savefile);
}

/*****************************************************************************

		TEXT_BOX FUNCTION

*****************************************************************************/

void text_box(int time, int event)
{  BITMAP* box = create_sub_bitmap(screen, 200, 200, 400, 200);
   BITMAP* back = create_bitmap(400, 200);
   BITMAP* buffer = create_bitmap(400, 200);
   blit (box, back, 0, 0, 0, 0, 400, 200);

	set_color_depth(8);
	BITMAP* stretch = create_bitmap(400, 200);
   set_color_depth(16);

   set_palette((PALETTE)data[BOXP0_PAL + time].dat);
   for (int i = 1; i <= 200; i += 4)
	{	stretch_blit((BITMAP*)data[BOX0_BMP + time].dat, stretch, 0, 0, 400, 200,
			(int)(200 - i), (int)(100 - i / 2), (int)(2 * i), (int)(i));
      blit(stretch, box, 200 - i, 100 - i / 2, 200 - i, 100 - i / 2, 2 * i, i);
		rest(1);
   }

   int line = 0;
   int element = 0;
   int column = 20;
   int skip = FALSE;
   int quit = FALSE;
   char word[20];

   set_palette((PALETTE)data[GREY_PAL].dat);
   text_mode(-1);

   rest(200);
   clear_keybuf();

   do
   {	sscanf((char*)data[EVENT000_TXT + event].dat + element, "%s", word);

   	if (column + text_length((FONT*)data[GOTHIC_FONT].dat, word) > 380)
      {	line++;
      	column = 20;
      }
      else if (strchr(word, 'ª') || strchr(word, 'œ'))
      {  clear_keybuf();
			rest(200);
         while (!key[KEY_ENTER] && !key[KEY_ESC] && !skip && !quit);
         if (key[KEY_ESC])
				quit = TRUE;
        	clear_keybuf();

      	line += 2;
         column = 20;
         skip = FALSE;
       	element += strlen(word) + 2;
      }
      else
		{	textout(box, (FONT*)data[GOTHIC_FONT].dat, word, column, 15 * line +
				20, -1);
         element += strlen(word) + 1;
         column += text_length((FONT*)data[GOTHIC_FONT].dat, word) + 12;

         if (keypressed())
         {	int kp = readkey() >> 8;
         	if (kp == KEY_ENTER)
         		skip = TRUE;
            else if (kp = KEY_ESC)
            	quit = TRUE;
         }

         if (!skip && !quit)
         	rest(200);
      }
	} while (!strchr(word, 'œ') && !quit);

   rest(200);

   set_palette((PALETTE)data[BOXP0_PAL + time].dat);
   for (int i = 200; i >= 1; i -= 4)
   {  blit (back, buffer, 0, 0, 0, 0, 400, 200);
		stretch_blit((BITMAP*)data[BOX0_BMP + time].dat, stretch, 0, 0, 400, 200,
			200 - i, 100 - i / 2, 2 * i, i);
		blit(stretch, buffer, 200 - i, 100 - i / 2, 200 - i, 100 - i / 2, 2 * i,
			i);
      blit(buffer, box, 0, 0, 0, 0, 400, 200);
		rest(1);
   }

   clear_keybuf();
}
