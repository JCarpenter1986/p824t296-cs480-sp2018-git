//FILE : main.cpp
//PROG : Dave Jones (with help of Jonathan Harbour textbook). Modified by Josh Carpenter for video game project
//PURP : A side scrolling game with Mappy support

#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_native_dialog.h>

//Include basic definitions/prototypes for mappy
#include "mappy_A5.h"

//three prototypes and header files
int collidedtl(int, int);
int collidedbl(int, int);
void wait_for_keypress();

#include "gameDefs.h"
#include "spriteClass.h"
#include "doorClass.h"
#include "mainDoorClass.h"
#include "mewtwoClass.h"
#include "srPokeBallsClass.h"
#include "shadowMewtwoClass.h"
#include "lightBlastPickupClass.h"
#include "maxReviveClass.h"
#include "maxPotionClass.h"
#include "bulletclass.h"
#include "BSM.h"
#include "spriteFunctions.h"

ALLEGRO_EVENT evt;
BossDoors *door[MAX_MB_DOORS];
MaxPotion *potions[MAX_POTIONS];
MaxRevive *revive;
/*________________________________________________________________________________________________________________________________*/

void wait_for_keypress()
{
  do
    al_wait_for_event(eventQueue, &evt);
  while (evt.type != ALLEGRO_EVENT_KEY_DOWN);
}

int main (void)
{
	bool redrawRequired = true, quit = false;
    int ct;	

	//Initialize allegro, etc
	if (!initializeAllegro())
	{
		return -1;
	}
/*________________________________________________________________________________________________________________________________*/
	if (!loadImages()) return -2;				/*80, 100		61632, 128*/
	player = new Mewtwo(61632, 128, true, al_get_bitmap_width(mewtwoJumping[0]), al_get_bitmap_height(mewtwoJumping[0]), 0, 1, JUMPIT, 0, 4, 0, 6);
	player = new Mewtwo(61632, 128, true, al_get_bitmap_width(mewtwoMoving[0]), al_get_bitmap_height(mewtwoMoving[0]), 0, 1, JUMPIT, 0, 5, 0, 6);	  
	player = new Mewtwo(61632, 128, true, al_get_bitmap_width(mewtwoIdle[0]), al_get_bitmap_height(mewtwoIdle[0]), 0, 1, JUMPIT, 0, 4, 0, 25);	
	
	
	for (ct = 0; ct < MAX_ENEMIES; ++ct)		/*test location x and y = 512		normal location 61600, 544*/
		enemies[ct] = new SRPokeBalls(rand() % 63000 + 800, rand() % 350, true, 
			al_get_bitmap_width(srPokeBalls), al_get_bitmap_height(srPokeBalls), 2, 0, 0, 0, 0, 0, 0);
	
	boss = new ShadowMewtwo(63712, 480, true, al_get_bitmap_width(sm_Idle[0]), al_get_bitmap_height(sm_Idle[0]), 0, 0, 0, 0, 4, 0, 25);
	
	pickup_TM101 = new LightBlastPickup(61600, 544, true, al_get_bitmap_width(pickupImage_TM101), al_get_bitmap_height(pickupImage_TM101),0,0,0,0,0,0,0);
		
	revive = new MaxRevive(26144, 544, true, al_get_bitmap_width(maxRevive), al_get_bitmap_height(maxRevive),0,0,0,0,0,0,0);		/*test location x and y = 512		normal location 26144, 544*/
		
	for (ct = 0; ct < MAX_MB_DOORS; ++ct)
	{
		door[0] = new BossDoors(13920, 288, false, al_get_bitmap_width(miniBossDoor), al_get_bitmap_height(miniBossDoor),0,0,0,0,0,0,0);
		door[1] = new BossDoors(14880, 288, false, al_get_bitmap_width(miniBossDoor), al_get_bitmap_height(miniBossDoor),0,0,0,0,0,0,0);
		door[2] = new BossDoors(29920, 288, false, al_get_bitmap_width(miniBossDoor), al_get_bitmap_height(miniBossDoor),0,0,0,0,0,0,0);
		door[3] = new BossDoors(30880, 288, false, al_get_bitmap_width(miniBossDoor), al_get_bitmap_height(miniBossDoor),0,0,0,0,0,0,0);
		door[4] = new BossDoors(45920, 288, false, al_get_bitmap_width(miniBossDoor), al_get_bitmap_height(miniBossDoor),0,0,0,0,0,0,0);
		door[5] = new BossDoors(46880, 288, false, al_get_bitmap_width(miniBossDoor), al_get_bitmap_height(miniBossDoor),0,0,0,0,0,0,0);
	}

	finalBossDoor = new MainDoor(62972, 236, true, al_get_bitmap_width(finalDoor[0]), al_get_bitmap_height(finalDoor[0]), 0,0,0,0,2,0,15);
	
	for (ct = 0; ct < MAX_POTIONS; ++ct)
	{
		potions[0] = new MaxPotion(19264, 416, true, al_get_bitmap_width(maxPotion), al_get_bitmap_height(maxPotion),0,0,0,0,0,0,0);
		potions[1] = new MaxPotion(35264, 416, true, al_get_bitmap_width(maxPotion), al_get_bitmap_height(maxPotion),0,0,0,0,0,0,0);
		potions[2] = new MaxPotion(51264, 416, true, al_get_bitmap_width(maxPotion), al_get_bitmap_height(maxPotion),0,0,0,0,0,0,0);
	}
/*________________________________________________________________________________________________________________________________*/
	//Load the map using a MappyAl function
	if (MapLoad("PokemonSOD.fmp", 1) != 0)
	{
		fprintf(stderr, "Failed to load the Mappy map.\n");
		return -7;
	}
	mapWidthInPixels = mapwidth * mapblockwidth;
	mapHeightInPixels = mapheight * mapblockheight;

	arial16 = al_load_font("arial-mt-black.ttf", 16, 0);
	if (!arial16)
	{
		fprintf(stderr, "Could not load arial font.\n");
		return -4;
	}
	arial24 = al_load_font("arial-mt-black.ttf", 24, 0);
	if (!arial24)
	{
		fprintf(stderr, "Could not load arial font.\n");
		return -4;
	}
/*________________________________________________________________________________________________________________________________*/

	//Set the back buffer as the drawing bitmap for the display
	al_set_target_bitmap(al_get_backbuffer(display));

	//Initialize the event queue
	eventQueue = al_create_event_queue();
	if (!eventQueue)
	{
		fprintf(stderr, "Failed to create event queue!\n");
		al_destroy_display(display);
		al_destroy_timer(timer);
		return -1;
	}

	//Register events as arriving from these sources:  display, timer, keyboard
	al_register_event_source(eventQueue, al_get_display_event_source(display));
	al_register_event_source(eventQueue, al_get_timer_event_source(timer));
	al_register_event_source(eventQueue, al_get_keyboard_event_source());
/*________________________________________________________________________________________________________________________________*/

	//display a title screen
	title = al_load_bitmap("Title.bmp");
	al_draw_bitmap(title, 0, 0, 0);
	al_flip_display();

	wait_for_keypress();

	story = al_load_bitmap("Story.bmp");
	al_draw_bitmap(story, 0, 0, 0);
	al_flip_display();

	wait_for_keypress();

	controls = al_load_bitmap("Controls.bmp");
	al_draw_bitmap(controls, 0, 0, 0);
	al_flip_display();
		
	wait_for_keypress();
/*________________________________________________________________________________________________________________________________*/
    //Central game loop
	while (!quit)
	{
		al_start_timer(timer);
		//Wait for one of the allegro-defined events
		al_wait_for_event(eventQueue, &evt);
/*________________________________________________________________________________________________________________________________*/
		//An event was generated. Check for all possible event types
		switch (evt.type)
		{
			case ALLEGRO_EVENT_KEY_DOWN:
				switch(evt.keyboard.keycode)
				{
					case ALLEGRO_KEY_ESCAPE:
						quit = true;
						break;
					case ALLEGRO_KEY_LEFT:
						keys[LEFT] = true;
						break;
					case ALLEGRO_KEY_RIGHT:
						keys[RIGHT] = true;
						break;
					case ALLEGRO_KEY_ALTGR:
						{
							jumpReleased = false;
							jumpCounter++;
							keys[ALT] = true;
						}
						break;
					case ALLEGRO_KEY_SPACE:
						keys[SPACE] = true;
						break;
					case ALLEGRO_KEY_V:
						keys[V] = true;
						break;
					case ALLEGRO_KEY_UP:
						keys[UP] = true;
						break;
					case ALLEGRO_KEY_DOWN:
						keys[DOWN] = true;
						break;
				}
				break;

			case ALLEGRO_EVENT_KEY_UP:
				switch(evt.keyboard.keycode)
				{
					case ALLEGRO_KEY_ESCAPE:
						quit = true;
						break;
					case ALLEGRO_KEY_LEFT:
						keys[LEFT] = false;
						break;
					case ALLEGRO_KEY_RIGHT:
						keys[RIGHT] = false;
						break;
					case ALLEGRO_KEY_ALTGR:
						{
							jumpReleased = true;
							keys[ALT] = false;
						}
						break;
					case ALLEGRO_KEY_SPACE:
						keys[SPACE] = false;
						break;
					case ALLEGRO_KEY_V:
						keys[V] = false;
						break;
					case ALLEGRO_KEY_UP:
						keys[UP] = false;
						break;
					case ALLEGRO_KEY_DOWN:
						keys[DOWN] = false;
						break;
				}
				break;

			case ALLEGRO_EVENT_DISPLAY_CLOSE:
				quit = true;
				break;
/*________________________________________________________________________________________________________________________________*/
			case ALLEGRO_EVENT_TIMER:
				//First, check all bullets
				bulletStack = cleanSprites (bulletStack, FALSE);
				//check for different types of collisions
				
				for (ct = 0; ct < MAX_ENEMIES; ++ct)
				if (enemies[ct]->getAlive())
				if (spriteCollide(player, enemies[ct]))
				{
					enemies[ct]->collideReboundEnemies();
					if (health > 0)
					{
						health -= 10;
						player->enemyCollision();
					}
				}
				if (boss->getAlive())
				if (spriteCollide(player, boss))
				{
					boss->collideReboundBoss();
					if (health > 0)
						health -= 20;
					if (health < 0)
						health = 0;
				}
		/*________________________________________________________________________________________________________________________________*/
				/* This section handles items that can be obtained */
				if (pickup_TM101->getAlive())
				if (spriteCollide(player, pickup_TM101))
				{
					newWeapon = true;
					pickup_TM101->kill();
				}
				if (revive->getAlive())
				if (spriteCollide(player, revive))
				{
					extraLife = true;
					revive->kill();
				}
				for (ct = 0; ct < MAX_POTIONS; ++ct)
				if (potions[ct]->getAlive())
				if (spriteCollide(player, potions[ct]))
				{
					health = 100;
					potions[ct]->kill();
				}
		/*________________________________________________________________________________________________________________________________*/
				//mini-boss door collision
				for (ct = 0; ct < MAX_MB_DOORS; ++ct)
				{
					if (spriteCollide(player, door[0]) || spriteCollide(player, door[1]))
					if (player->getX()>=13930 && player->getX() <= 14880)
					player->setNewX();
					if (spriteCollide(player, door[2]) || spriteCollide(player, door[3]))
					if (player->getX()>=29930 && player->getX() <= 30880)
					player->setNewX();
					if (spriteCollide(player, door[4]) || spriteCollide(player, door[5]))
					if (player->getX()>=45930 && player->getX() <= 46880)
					player->setNewX();
				}
				//main boss door collision
				if (finalBossDoor->getAlive())
				if (spriteCollide(player, finalBossDoor))
					player->bossDoorCollision();
		/*________________________________________________________________________________________________________________________________*/					
				//Update the player's movement & collision status
				player->handleKeyboard();								

				//update the map scroll position
				mapxoff = player->getX() + player->getWidth()/2 - SCRN_W/2 + 10;
				mapyoff = player->getY() + player->getHeight()/2 - SCRN_H/2 + 10;

				//Avoid moving beyond the map edge
				if (mapxoff < 0) mapxoff = 0;
				if (mapxoff > (mapwidth * mapblockwidth - SCRN_W))
					mapxoff = mapwidth * mapblockwidth - SCRN_W;
				if (mapyoff < 0) 
					mapyoff = 0;
				if (mapyoff > (mapheight * mapblockheight - SCRN_H)) 
					mapyoff = mapheight * mapblockheight - SCRN_H;

				//update bulletstack
				updateBeams(bulletStack);

				redrawRequired = true;
				break;

		}//END switch evt.type
		/*________________________________________________________________________________________________________________________________*/

		if (redrawRequired && al_is_event_queue_empty(eventQueue))
		{
			redrawRequired = false;

			//Draw the background & foreground tiles
			MapDrawBG(mapxoff, mapyoff, 0, 0, SCRN_W-1, SCRN_H-1);
			MapDrawFG(mapxoff, mapyoff, 0, 0, SCRN_W-1, SCRN_H-1, 0);

			//Draw the player's sprite
			player->drawPlayer(mapxoff, mapyoff);
		/*________________________________________________________________________________________________________________________________*/
			//can't go past doors until the mini-boss is beaten
			//Electrode's room
			if (player->getX()>=13930 && player->getX() <= 14880)
			if (!miniBosses[BEATEN1])
			{
				door[0]->drawDoors(mapxoff, mapyoff);
				door[1]->drawDoors(mapxoff, mapyoff);
			}
			//Bowser's room
			if (player->getX()>=29930 && player->getX() <= 30880)
			if (!miniBosses[BEATEN2])
			{
				door[2]->drawDoors(mapxoff, mapyoff);
				door[3]->drawDoors(mapxoff, mapyoff);
			}
			//Giant Red Metroid's room
			if (player->getX()>=45930 && player->getX() <= 46880)
			if (!miniBosses[BEATEN3])
			{
				door[4]->drawDoors(mapxoff, mapyoff);
				door[5]->drawDoors(mapxoff, mapyoff);
			}
			//Shadow Mewtwo's room
			if (finalBossDoor->getAlive())
			{	
				finalBossDoor->updateMainDoor();
				finalBossDoor->drawMainDoor(mapxoff, mapyoff);
			}
		/*________________________________________________________________________________________________________________________________*/
			//Draw all bullets
			drawBullets (bulletStack);
			//update and draw enemies
			for (ct = 0; ct < MAX_ENEMIES; ++ct)
			if (enemies[ct]->getAlive())
			{
				enemies[ct]->updateEnemies();
				enemies[ct]->drawEnemies(mapxoff, mapyoff);				
			}
			//update and draw boss
			if (boss->getAlive())
			{
				boss->updateBoss();
				boss->drawBoss(mapxoff, mapyoff);
			}
			//draw TM 101's pickup sprite
			pickup_TM101->drawTM101(mapxoff, mapyoff);
			//draw the max revive's pickup sprite
			revive->drawMaxRevive(mapxoff, mapyoff);
			//draw the three max potions
			for (ct = 0; ct < MAX_POTIONS; ++ct)
			{
				potions[ct]->drawMaxPotion(mapxoff, mapyoff);
			}
		/*________________________________________________________________________________________________________________________________*/
			
			//display health bar and stats
			displayBars(health, recoverPercent);
			displayStats();
			//display game over screen when health reaches zero
			if (health == 0)
			if (extraLife)
			{
				health = 100;
				extraLife = false;
			}
			else
				displayGameOver();

			al_flip_display();
		}
	}//END while !quit
/*________________________________________________________________________________________________________________________________*/
	//Release all dynamically allocated memory
	//First, free up all the allocated memory for bullets
	bulletStack = cleanSprites (bulletStack, TRUE);

	MapFreeMem();

	//delete objects
	delete player;	
	delete boss;
	delete finalBossDoor;
	delete pickup_TM101;
	delete revive;

	//delete images that have multiple frames
	for (ct = 0; ct < MAX_ENEMIES; ++ct)
		delete enemies[ct];
	for (ct = 0; ct < MAX_MB_DOORS; ++ct)
		delete door[ct];
	for (ct = 0; ct < MAX_POTIONS; ++ct)
		delete potions[ct];
	for (ct = 0; ct < 6; ++ct)
		al_destroy_bitmap(mewtwoMoving[ct]);
	for (ct = 0; ct < 5; ++ct)
		al_destroy_bitmap(mewtwoIdle[ct]);
	for (ct = 0; ct < 5; ++ct)
		al_destroy_bitmap(sm_Idle[ct]);	
	for (ct = 0; ct < 5; ++ct)
		al_destroy_bitmap(mewtwoJumping[ct]);
	for (ct = 0; ct < 3; ++ct)
		al_destroy_bitmap(finalDoor[ct]);
/*________________________________________________________________________________________________________________________________*/
	//delete images that only have one frame
	al_destroy_bitmap(srPokeBalls);
	al_destroy_bitmap(shadowBall);
	al_destroy_bitmap(lightBlast);	
	al_destroy_bitmap(pickupImage_TM101);
	al_destroy_bitmap(maxPotion);
	al_destroy_bitmap(maxRevive);
	al_destroy_bitmap(miniBossDoor);
	al_destroy_bitmap(title);
	al_destroy_bitmap(story);
	al_destroy_bitmap(controls);
	al_destroy_bitmap(gameOver);
	al_destroy_bitmap(progress);
	al_destroy_bitmap(bar);
	al_destroy_bitmap(recover);
	al_destroy_bitmap(bar2);
	al_destroy_bitmap(itemBox);
	al_destroy_bitmap(itemBoxFull);
	//delete non-bitmaps
	al_destroy_display(display);
	al_destroy_timer(timer);
	al_destroy_event_queue(eventQueue);
	al_destroy_font(arial16);
	al_destroy_font(arial24);
	//Free memory allocated to the Mappy map	

	return 0;
}//END main
/*________________________________________________________________________________________________________________________________*/

bool initializeAllegro()
{
	bool success = true;

	//Init basic environment
	if (!al_init())
	{
		al_show_native_message_box(NULL, "ERROR", "Allegro failed to initialize!", NULL, NULL, NULL);
		success = false;
	}

	//Initialize keyboard input
	if (!al_install_keyboard())
	{
		al_show_native_message_box(NULL, "ERROR", "Keyboard failed to initialize!", NULL, NULL, NULL);
		success = false;
	}

	//Initialize timer
	timer = al_create_timer(1.0 / FPS);
	if (!timer)
	{
		al_show_native_message_box(NULL, "ERROR", "Timer failed to initialize!", NULL, NULL, NULL);
		success = false;
	}

	//Initialize display
	display = al_create_display(SCRN_W, SCRN_H);
	if (!display)
	{
		al_show_native_message_box(NULL, "ERROR", "Display failed to initialize!", NULL, NULL, NULL);
		success = false;
	}

	//Initialize the image mechanism
	if (!al_init_image_addon())
	{
		al_show_native_message_box(NULL, "ERROR", "Image addon failed to initialize!", NULL, NULL, NULL);
		success = false;
	}

	al_init_font_addon();
	al_init_ttf_addon();

	return success;
}//END initializeAllegro
/*________________________________________________________________________________________________________________________________*/

int collidedtl(int x, int y)
{
    BLKSTR *blockdata;
	blockdata = MapGetBlock(x/mapblockwidth, y/mapblockheight);
	return blockdata->tl;
}//END collidedtl
int collidedbl(int x, int y)
{
	BLKSTR *blockdata;
	blockdata = MapGetBlock(x/mapblockwidth, y/mapblockheight);
	return blockdata->bl;
}//END collidedbl