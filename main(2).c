#include <stm32f031x6.h>
#include <stdlib.h>
#include "display.h"
#include "sound.h"
#include "musical_notes.h"
#include "font5x7.h"
#include "stm32f0xx.h"  // Ensure you include the STM32 header
#include <stdint.h>
#include <stdio.h>
uint32_t shift_register = 0;

struct zombies
{
	float zombx;
	float zomby;
	int zombnum;
};

void initClock(void);
void initSysTick(void);
void SysTick_Handler(void);
void delay(uint32_t dly);
uint32_t prbs();
void setupIO();
int isInside(uint16_t x1, uint16_t y1, uint16_t w, uint16_t h, uint16_t px, uint16_t py, uint16_t padding);
void enablePullUp(GPIO_TypeDef *Port, uint32_t BitNumber);
void pinMode(GPIO_TypeDef *Port, uint32_t BitNumber, uint32_t Mode);
void updateNotePlayback();
void testsound();
uint32_t randomise(uint32_t,uint32_t);
uint32_t millis(void);
void updateNotePlayback();
volatile uint32_t milliseconds;
uint32_t noteStartTime = 0;
int noteDuration = 0;

//void delay_ms(uint32_t ms);
void playNoteWithDuration(uint32_t freq, int duration);
void greenOn(void);
void greenOff(void);
void redOn(void);
void redOff(void);
void yellowOn(void);
void yellowOff(void);

// Theme functions
void playWinTheme();
void playPVZTheme(void);
void playLoseTheme();

// checking player health
void checkhealth(int,uint16_t,uint16_t,int);

// mainmenu function
int mainmenu(void);

// ------------------------------------------------------------------------------ GRAPHICS

// -------------------------------------- Graphics for black 16x16
const uint16_t black[]= //black 16x16
{
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

// -------------------------------------- Graphics for shooter's pellet (bullet)
const uint16_t pellet[]= //pellet shot
{
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,9293,9293,0,0,0,0,0,0,0,0,0,0,0,0,0,9293,54559,54559,9293,0,0,0,0,0,0,0,0,0,0,0,0,9293,54559,54559,9293,0,0,0,0,0,0,0,0,0,0,0,0,0,9293,9293,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

// -------------------------------------- Graphics for zombie

const uint16_t zombie[]= //zombie
{
	0,0,0,51266,0,58674,58674,58674,58674,58674,17441,17441,0,0,0,0,0,0,0,0,51266,51803,53124,53124,53124,53124,53124,53124,17441,0,51266,0,0,0,0,58674,51803,53124,53124,53124,53124,58674,58674,58674,53124,51266,0,0,0,0,51266,51266,58674,58674,53124,53124,53124,24527,24527,24527,53124,58674,0,0,0,0,0,58674,49448,24527,53124,53124,53124,24527,49448,24527,53124,51266,51266,0,0,0,0,58674,24527,24527,53124,53124,53124,24527,24527,24527,19035,58674,0,0,0,0,0,58674,19035,19035,53124,53124,53124,19035,19035,19035,53124,58674,0,0,0,0,0,0,0,65535,2584,65535,2584,53124,53124,53124,17441,0,0,0,0,0,0,0,0,2584,2584,2584,2584,2584,53124,58674,58674,0,0,0,0,0,0,0,0,65535,2584,2584,65535,2584,53124,58674,0,0,0,0,0,0,0,0,0,34064,17441,17441,17441,17441,58674,34064,0,0,0,0,0,0,0,0,2073,2073,34064,65535,4912,65535,34064,2073,2073,0,0,0,0,0,53124,2073,2073,0,34064,65535,4912,65535,34064,0,2073,0,0,0,0,0,0,0,0,0,42305,34658,34658,42305,42305,0,53124,0,0,0,0,0,0,0,0,42305,34658,0,0,0,53124,0,0,0,0,0,0,0,0,0,34064,34064,0,0,0,34064,34064,0,0,0,0,0,
};

const uint16_t zombieF2[] = //zombie frame 2
{
	0,0,0,51266,0,58674,58674,58674,58674,58674,17441,17441,0,0,0,0,0,0,0,0,51266,51803,53124,53124,53124,53124,53124,53124,17441,0,51266,0,0,0,0,58674,51803,53124,53124,53124,53124,58674,58674,58674,53124,51266,0,0,0,0,51266,51266,58674,58674,53124,53124,53124,24527,24527,24527,53124,58674,0,0,0,0,0,58674,49448,24527,53124,53124,53124,24527,49448,24527,53124,51266,51266,0,0,0,0,58674,24527,24527,53124,53124,53124,24527,24527,24527,19035,58674,0,0,0,0,0,58674,19035,19035,53124,53124,53124,19035,19035,19035,53124,58674,0,0,0,0,0,0,0,65535,2584,65535,2584,53124,53124,53124,17441,0,0,0,0,0,0,0,0,2584,2584,2584,2584,2584,53124,58674,58674,0,0,0,0,0,0,0,0,65535,2584,2584,65535,2584,53124,58674,0,0,0,0,0,0,0,0,0,34064,17441,17441,17441,17441,58674,34064,0,0,0,0,0,0,0,0,2073,2073,34064,65535,4912,65535,34064,2073,2073,0,0,0,0,0,53124,2073,2073,0,34064,65535,4912,65535,34064,0,2073,0,0,0,0,0,0,0,0,0,42305,34658,34658,42305,42305,0,53124,0,0,0,0,0,0,0,0,0,34658,0,0,0,0,53124,0,0,0,0,0,0,0,0,0,0,0,2073,0,0,34064,2073,0,0,0,0,
};

// -------------------------------------- Graphics for shoooter
const uint16_t shooter[]= //shooter
{
	0,0,9293,0,0,0,0,0,0,0,0,0,0,0,0,0,0,9293,9293,0,0,54559,54559,54559,54559,0,0,0,0,0,0,0,9293,9293,9293,9293,11542,54559,54559,54559,54559,54559,0,0,0,0,0,0,0,9293,9293,11542,54559,54559,54559,54559,54559,54559,54559,0,0,0,9293,0,0,9293,0,11542,54559,54559,54559,54559,54559,0,54559,54559,54559,9293,54559,9293,0,9293,0,11542,54559,54559,54559,54559,54559,54559,54559,54559,54559,9293,54559,9293,0,0,0,11542,54559,54559,54559,54559,54559,54559,54559,0,0,0,9293,0,0,0,0,0,11542,54559,54559,54559,54559,54559,0,0,0,0,0,0,0,0,0,0,0,11542,54559,54559,54559,0,0,0,0,0,0,0,0,0,0,0,0,0,11542,11542,0,0,0,0,0,0,0,0,0,0,9293,0,9293,0,11542,54559,0,9293,0,9293,0,0,0,0,0,0,0,9293,9293,0,11542,54559,0,9293,9293,0,0,0,0,0,0,0,0,9293,9293,0,11542,54559,0,9293,9293,0,0,0,0,0,0,0,0,0,9293,9293,11542,54559,9293,9293,0,0,0,0,0,0,0,0,0,0,0,9293,9293,9293,9293,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};
const uint16_t pea2hp[]= //shooter at 2p 
{
	0,0,4890,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4890,4890,0,0,24334,24334,24334,24334,0,0,0,0,0,0,0,4890,4890,4890,4890,38659,24334,24334,24334,24334,24334,0,0,0,0,0,0,0,4890,4890,38659,24334,24334,24334,24334,24334,24334,24334,0,0,0,38659,0,0,4890,0,38659,24334,24334,24334,24334,24334,0,24334,24334,24334,38659,24334,38659,0,4890,0,38659,24334,24334,24334,24334,24334,24334,24334,24334,24334,38659,24334,38659,0,0,0,38659,24334,24334,24334,24334,24334,24334,24334,0,0,0,38659,0,0,0,0,0,38659,24334,24334,24334,24334,24334,0,0,0,0,0,0,0,0,0,0,0,38659,24334,24334,24334,0,0,0,0,0,0,0,0,0,0,0,0,0,38659,38659,0,0,0,0,0,0,0,0,0,0,4890,0,4890,0,38659,24334,0,4890,0,4890,0,0,0,0,0,0,0,4890,4890,0,38659,24334,0,4890,4890,0,0,0,0,0,0,0,0,4890,4890,0,38659,24334,0,4890,4890,0,0,0,0,0,0,0,0,0,4890,4890,38659,24334,4890,4890,0,0,0,0,0,0,0,0,0,0,0,4890,4890,4890,4890,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};
const uint16_t pea1hp[]= //shooter at 1hp
{
	0,0,35601,0,0,0,0,0,0,0,0,0,0,0,0,0,0,35601,35601,0,0,62218,62218,62218,62218,0,0,0,0,0,0,0,35601,35601,35601,35601,27905,62218,62218,62218,62218,62218,0,0,0,0,0,0,0,35601,35601,27905,62218,62218,62218,62218,62218,62218,62218,0,0,0,27905,0,0,35601,0,27905,62218,62218,62218,62218,0,0,62218,62218,62218,27905,62218,27905,0,35601,0,27905,62218,62218,62218,62218,62218,62218,62218,62218,62218,27905,62218,27905,0,0,0,27905,62218,62218,62218,62218,62218,62218,62218,0,0,0,27905,0,0,0,0,0,27905,62218,62218,62218,62218,62218,0,0,0,0,0,0,0,0,0,0,0,27905,62218,62218,62218,0,0,0,0,0,0,0,0,0,0,0,0,0,27905,27905,0,0,0,0,0,0,0,0,0,0,35601,0,35601,0,27905,62218,0,35601,0,35601,0,0,0,0,0,0,0,35601,35601,0,27905,62218,0,35601,35601,0,0,0,0,0,0,0,0,35601,35601,0,27905,62218,0,35601,35601,0,0,0,0,0,0,0,0,0,35601,35601,27905,62218,35601,35601,0,0,0,0,0,0,0,0,0,0,0,35601,35601,35601,35601,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};
const uint16_t grave[]= //shooter at 0hp
{
	0,0,0,0,30125,30125,38581,38581,38581,22197,36452,36452,36452,0,0,0,0,0,0,19268,30125,20332,30125,62893,11076,35652,52036,43308,50979,11083,0,0,0,0,19268,2884,2884,61036,5549,13477,37252,52036,52036,35652,36452,38581,53115,0,0,0,52036,2884,30125,46245,20332,21933,62893,46509,10804,37252,20596,38581,53115,0,0,30125,19268,30125,21405,46245,30125,21933,28524,12141,52836,38581,5549,38581,53636,19547,0,20332,34353,51266,51266,30125,26161,51266,51266,38581,9777,51266,51266,12141,38581,53115,0,62893,26161,30125,51266,20332,38581,51266,38581,38581,9777,38581,51266,38581,38581,53115,0,22197,17969,17969,51266,22197,22197,51266,38581,38581,9777,51266,38581,38581,38581,53115,0,38581,9777,50737,38581,14005,38581,9777,38581,38581,9777,38581,62893,62893,62893,28539,0,37789,17969,14005,9777,14005,9777,9777,51266,22197,9777,38581,38581,38581,38581,53115,0,37789,14005,60492,38581,46245,37789,38581,62893,22197,22197,38581,62893,30125,30125,20347,0,19796,37788,46245,46245,38581,14005,38581,38581,46509,62893,5549,5549,5549,30125,3963,0,37788,53108,52036,5549,60492,38581,62092,22197,14005,62893,30125,5549,5549,30125,20347,0,3420,52036,18996,52036,37788,60492,22197,46245,21933,5549,46245,62893,62893,30125,20347,0,43308,43308,18996,52036,52036,60492,37788,46245,22197,30125,62893,62893,30125,30125,3963,0,36452,52036,52036,52036,38581,38581,22197,62893,62893,62893,5549,5549,30125,30125,52851,
};


// ------------------------------------------------------------------------------ MAIN()

int main()
{
	initClock();
	initSysTick();
	setupIO();
	greenOn();
	yellowOn();
	redOn();
	int difficulty = 0;
	float zombieSpeed = 0;

	// calls main menu function
	difficulty = mainmenu();	

	// returned difficulty value decides the speed of zombies
	if (difficulty == 1)
	{
		zombieSpeed = 0.3;
	}// end if (difficulty == 1)

	if (difficulty == 2)
	{
		zombieSpeed = 0.6;
	}// end if (difficulty == 2)

	if (difficulty == 3)
	{
		zombieSpeed = 1.1;
	}// end if (difficulty == 3)


	// while is game is on
	while(1)
	{
		int gamecondition = 0;
		int toggle = 0;
		int vmoved = 0;
		int tempx = 0;
		int random = -1;
		int zombieDead = 0;
		int count = 0;
		float z1 = 0;
		float z2 = 0;
		float z3 = 0;
		float z4 = 0;
		int winThemePlayed = 0;
		int loseThemePlayed = 0;
		int playerhealth = 0;
		int flash = 0;
		
		uint16_t x = 20;
		uint16_t y = 80;
		uint16_t oldx = x;
		uint16_t oldy = y;
		uint16_t padding = 5; // padding around hitbox 

		putImage(20,80,16,16,shooter,0,0); //displays player starting position

		struct zombies zombie1;
		struct zombies zombie2;
		struct zombies zombie3;
		struct zombies zombie4;

		zombie1.zombx = 100; // starting x position of zombie1
		zombie1.zomby = 20; // starting y position of zombie1 
		zombie1.zombnum = 1; // initialising the row/zombie number

		zombie2.zombx = 100; // starting x position of zombie2
		zombie2.zomby = 60; // starting y position of zombie2
		zombie2.zombnum = 2; // initialising the row/zombie number

		zombie3.zombx = 100; // starting x position of zombie3
		zombie3.zomby = 100; // starting y position of zombie3
		zombie3.zombnum = 3; // initialising the row/zombie number

		zombie4.zombx = 100; // starting x position of zombie4
		zombie4.zomby = 140; // starting y position of zombie4
		zombie4.zombnum = 4; // initialising the row/zombie number

		z1 = zombie1.zombx;
		z2 = zombie1.zombx;
		z3 = zombie1.zombx;
		z4 = zombie1.zombx;

		// keeps the old zombies x coord
		int oldZombieX = 0;
		int oldZombie2X = 0;
		int oldZombie3X = 0;
		int oldZombie4X = 0;

		// keeps track how many times a zombie moves
		int countermove1 = 0;
		int countermove2 = 0;
		int countermove3 = 0;
		int countermove4 = 0;
		
		// players health starts at 3
		playerhealth = 3;

		// each light on represents a players health 
		redOn();
		yellowOn();
		greenOn();

		// if user presses left button, game condition = 1
		if ((GPIOB->IDR & (1 << 5))==0)
		{
			gamecondition = 1;

		}// end if 

			// while game condition is 1, play game
			while(gamecondition == 1)
			{
				// count = the amount of zombies dead, choose a random zombie and count++
				if (zombieDead == count)
				{
					random = randomise(1,6); // chooses a random number to spawn a zombie
					count++;

					switch(zombieDead)//switch to display amount of zombies left. checks zombie dead
					{
						case 0:
							{
								printText("zom left: 10", 10, 5, RGBToWord(255,0,0),0);
								break;
							}

						case 1:
							{
								printText("zom left: 9 ", 10, 5, RGBToWord(255,0,0),0);
								break;
							}
						
						case 2:
							{
								printText("zom left: 8 ", 10, 5, RGBToWord(255,0,0),0);
								break;
							}
							
						case 3:
							{
								printText("zom left: 7 ", 10, 5, RGBToWord(255,0,0),0);
								break;
							}
						case 4:
							{
								printText("zom left: 6 ", 10, 5, RGBToWord(255,0,0),0);
								break;
							}
						case 5:
							{
								printText("zom left: 5 ", 10, 5, RGBToWord(255,0,0),0);
								break;
							}
						case 6:
							{
								printText("zom left: 4 ", 10, 5, RGBToWord(255,0,0),0);
								break;
							}
						case 7:
							{
								printText("zom left: 3 ", 10, 5, RGBToWord(255,0,0),0);
								break;
							}
						
						case 8:
							{
								printText("zom left: 2 ", 10, 5, RGBToWord(255,0,0),0);
								break;
							}
						
						case 9:
							{
								printText("zom left: 1 ", 10, 5, RGBToWord(255,0,0),0);
								break;
							}
						
						case 10:
							{
								printText("zom left: 0 ", 10, 5, RGBToWord(255,0,0),0);
								break;
							}
						default:
								printText("zom left: idk", 10, 5, RGBToWord(255,0,0),0);//defauly for it if breaks
					}
				}// end if (zombieDead == count)

				// ---------------------------------------------------------- IF CHOSEN ZOMBIE = 1
				if (zombie1.zombnum == random)
				{
					oldZombieX = z1;
					z1 = z1 - zombieSpeed; // Move zombie1 to the left
					
					putImage(oldZombieX,zombie1.zomby, 16,16,black,0,0);
					putImage(z1, zombie1.zomby, 16, 16, zombie, 0, 0);
					countermove1 = countermove1 +  1;

					// to have a second frame of zombie moving (animate)
					if (countermove1 % 3 == 0)
					{
						putImage(oldZombieX,zombie1.zomby, 16,16,black,0,0);
						putImage(z1,zombie1.zomby, 16,16,zombieF2,0,0);
					} // end if counter mod 3 = 0

				} // end if (zombie1.zombnum == random)

				// ---------------------------------------------------------- IF CHOSEN ZOMBIE = 2
				else if (zombie2.zombnum == random)
				{
					oldZombie2X = z2;
					z2 = z2 - zombieSpeed; // Move zombie2 to the left
					putImage(oldZombie2X,zombie2.zomby, 16,16,black,0,0);
					putImage(z2, zombie2.zomby, 16, 16, zombie, 0, 0);
					countermove2 = countermove2 +  1;

					// to have a second frame of zombie moving (animate)
					if (countermove2 % 3 == 0)
					{
						putImage(oldZombie2X,zombie1.zomby, 16,16,black,0,0);
						putImage(z2,zombie2.zomby, 16,16,zombieF2,0,0);
					} // end if counter mod 3 = 0

				} // end else if (zombie2.zombnum == random)

				// ---------------------------------------------------------- IF CHOSEN ZOMBIE = 3
				else if (zombie3.zombnum == random)
				{
					oldZombie3X = z3;
					z3 = z3 - zombieSpeed; // Move zombie3 to the left
					putImage(oldZombie3X,zombie3.zomby, 16,16,black,0,0);
					putImage(z3, zombie3.zomby, 16, 16, zombie, 0, 0);
					countermove3 = countermove3 +  1;

					// to have a second frame of zombie moving (animate)
					if (countermove3 % 3 == 0)
					{
						putImage(oldZombie3X,zombie3.zomby, 16,16,black,0,0);
						putImage(z3,zombie3.zomby, 16,16,zombieF2,0,0);
					} // end if counter mod 3 = 0

				} // end else if (zombie3.zombnum == random)

				// ---------------------------------------------------------- IF CHOSEN ZOMBIE = 4
				else if (zombie4.zombnum == random)
				{
					oldZombie4X = z4;
					z4 = z4 - zombieSpeed; // Move zombie1 to the left by 0.3 pixel per frame
					putImage(oldZombie4X,zombie4.zomby, 16,16,black,0,0);
					putImage(z4, zombie4.zomby, 16, 16, zombie, 0, 0);
					countermove4 = countermove4 +  1;

					// to have a second frame of zombie moving (animate)
					if (countermove4 % 3 == 0)
					{
						putImage(oldZombie4X,zombie4.zomby, 16,16,black,0,0);
						putImage(z4,zombie4.zomby, 16,16,zombieF2,0,0);
					} // end if counter mod 3 = 0

				} // end else if (zombie4.zombnum == random)

// --------------------------------------------------------------------------------------------------- WIN	

				// if 10 zombies killed, player wins
				if (zombieDead == 10)
				{
					// if win theme HAS NOT played
					if (winThemePlayed == 0) 
					{
						initSound(); 
						printTextX2("YOU WIN!", 10, 20, RGBToWord(0xff,0xff,0), 0);
						random = 100;
						gamecondition = 0;
						putImage(z1, zombie1.zomby, 16, 16, black, 0, 0);
						putImage(z2, zombie2.zomby, 16, 16, black, 0, 0);
						putImage(z3, zombie3.zomby, 16, 16, black, 0, 0);
						putImage(z4, zombie4.zomby, 16, 16, black, 0, 0);
						playWinTheme();
						fillRectangle(0,0,128,160,0);
						gamecondition = 0; // make game stop 
						mainmenu(); // go back to main menu 
						winThemePlayed = 1; // Set the flag to indicate the win theme has been played
					}// end if win theme HAS NOT played

				}// end if (zombiesDead == 10)
				
// --------------------------------------------------------------------------------------------------- LOSE

				// if ANY zombie has an x == 20
				if (z1 < 20 || z2 < 20 || z3 < 20 || z4 < 20)
				{
					// make zombies disappear when they reach x = 20
					if (z1 < 20)
					{
						putImage(z1, zombie1.zomby, 16, 16, black, 0, 0);
					}
					if (z2 < 20)
					{
						putImage(z2, zombie2.zomby, 16, 16, black, 0, 0);
					}
					if (z3 < 20)
					{
						putImage(z3, zombie3.zomby, 16, 16, black, 0, 0);
					}
					if (z4 < 20)
					{
						putImage(z4, zombie4.zomby, 16, 16, black, 0, 0);
					}

					// make zombies go back to the start, x = 100
					z1 = zombie1.zombx;
					z2 = zombie2.zombx;
					z3 = zombie3.zombx;
					z4 = zombie4.zombx;
					
					// if health isnt 0, decrement it by 1
					if (playerhealth != 0)
					{
						playerhealth = playerhealth - 1;

					}// end if (health != 0)

					// if health is 0, player loses
					if (playerhealth == 0)
					{
						if (loseThemePlayed == 0)
						{
							putImage(x,y,16,16,grave,0,0);
							greenOff();
							yellowOff();
							redOff();
							initSound();
							printTextX2("GAMEOVER!", 10, 20, RGBToWord(0xff,0xff,0), 0);
							
							playLoseTheme();
							fillRectangle(0,0,128,160,0);
							gamecondition = 0;// end game
							mainmenu();// go to menu
							loseThemePlayed = 1;// Set the flag to indicate the lose theme has been played
						}// end if (loseThemePlayed == 0)

					}// end if (health is 0)
					
				}// end if any zombie x is 20
				
				checkhealth(playerhealth,x,y,flash); // checks players health 
						
				vmoved = 0; //resets if player moved so it doesnt stay in loop

// --------------------------------------------------------------------------------- IF RIGHT PRESSED > (SHOOTING)

				if ((GPIOB->IDR & (1 << 4)) == 0 ) // right pressed
				{	
					initSound();  // Initialize sound
					putImage((x+20),y,16,16,pellet,0,0); // puts pellet 20 spots away from peashooter to ensure wont make it disappear
					tempx = x; // stores x
					x = x+20; // moves pellet out

					while (x < 100) // while != isinside? (so pellet is not going til end)
					{
						x = x + 1;
						delay(20);
						putImage(x, y, 16, 16, pellet, 0, 0); // move pellet forward

						//if pellet is in zombie lane 1
						if (isInside(z1, zombie1.zomby, 16, 16, x, y, 5)) 
						{
							putImage(z1, zombie1.zomby, 16, 16, black, 0, 0); //places black over zombie start -> make into zombies current location
							z1 = zombie1.zombx;
							zombieDead = zombieDead + 1;
							break;
						}// end if pellet in zombie lane 1

						//if pellet is in zombie lane 2
						if (isInside(z2, zombie2.zomby, 16, 16, x, y, 5)) 
						{
							putImage(z2, zombie2.zomby, 16, 16, black, 0, 0);
							z2 = zombie2.zombx;
							zombieDead = zombieDead + 1;
							break;
						}// end if pellet is in zombie lane 2

						//if pellet is in zombie lane 3
						if (isInside(z3, zombie3.zomby, 16, 16, x, y, 5)) 
						{
							putImage(z3, zombie3.zomby, 16, 16, black, 0, 0);
							z3 = zombie3.zombx;
							zombieDead = zombieDead + 1;
							break;
						}// end if pellet is in zombie lane 3

						//if pellet is in zombie lane 4
						if (isInside(z4, zombie4.zomby, 16, 16, x, y, 5)) 
						{
							putImage(z4, zombie4.zomby, 16, 16, black, 0, 0);
							z4 = zombie4.zombx;
							zombieDead = zombieDead + 1;
							break;
						}// end if pellet is in zombie lane 4
						
						

					}// end while(x<100)

					x = tempx; //restores x position for player	

				}// end if right is pressed

// --------------------------------------------------------------------------------- IF LEFT PRESSED <
// ! Not used in the game, but kept just incase !
				/*
				if ((GPIOB->IDR & (1 << 5))==0) // left pressed
				{			
					
					if (x > 10)
					{
						x = x - 1;
						hmoved = 1;
						hinverted=1;
					}			
				} 
				*/
// --------------------------------------------------------------------------------- IF DOWN PRESSED v
				if ( (GPIOA->IDR & (1 << 11)) == 0 ) // down pressed
				{
					if (y < 140)
					{
						y = y + 3;			
						vmoved = 1;
					}// end inner if

				}// end if
// --------------------------------------------------------------------------------- IF UP PRESSED ^
				if ( (GPIOA->IDR & (1 << 8)) == 0 ) // up pressed
				{			
					if (y > 16)
					{
						y = y - 3;
						vmoved = 1;
					}// end inner if

				}// end if
// --------------------------------------------------------------------------------- IF VMOVED
				if ((vmoved))
				{
					// only redraw if there has been some movement (reduces flicker)
					fillRectangle(oldx,oldy,16,16,0);
					oldx = x; //keeps position
					oldy = y;					
					if (vmoved == 1) //checks if player used ^ or v
					{
						if (toggle)
						{

							putImage(x,y,16,16,shooter,0,0); //puts image in new pos

						}// end inner inner if

						toggle = toggle ^ 1;

					}// end inner if 
					else
					{
						//putImage(x,y,16,16,deco3,0,0);

					}// end inner else

					// Now check for an overlap by checking to see if ANY of the 4 corners of deco are within the target area
					
				}// end if	

				delay(50);

			}// end while(gamecondition)


	}// end while(1)


	return 0;


}// end main()

// --------------------------------------------------------------------------------- MAIN MENU

int mainmenu(void)
{	
	putImage(20,80,16,16,shooter,0,0); //displays player starting position
	int dif = 0;
	int menu = 1; //in menu
	
	// while in menu 
	while(menu == 1)
	{
		
		if ((GPIOB->IDR & (1 << 5))==0)// if left is pressed < (if difficulty is chosen)
		{
			// if dif selected isnt 0 or 4, exit menu
			if (dif != 0 && dif != 4)
			{
				menu = 0; //exit menu	
			}// end if (dif != 0 && dif != 4)
			else
			{
				printText("PLEASE SELECT", 10, 100, RGBToWord(0,0,0), 0);
				printText("DIFFICULTY", 10, 120, RGBToWord(0,0,0), 0);
				printText("PLEASE SELECT", 10, 100, RGBToWord(255,0,0), 0);
				printText("DIFFICULTY", 10, 120, RGBToWord(255,0,0), 0);
				delay(50);
				printText("PLEASE SELECT", 10, 100, RGBToWord(0,0,0), 0);
				printText("DIFFICULTY", 10, 120, RGBToWord(0,0,0), 0);
			}// end else
		}//end if left is pressed < (if difficulty is chosen)


		// if up pressed ^
		if ((GPIOA->IDR & (1 << 8))==0) 
		{
			if (dif < 4)
    		{
        		dif++;
   			}
    		while ((GPIOA->IDR & (1 << 8)) == 0);
		}// end if up pressed

		
		// down pressed v
		if ((GPIOA->IDR & (1 << 11))==0) 
		{
			if (dif > 0)
    		{
        		dif--;
   			}
    		while ((GPIOA->IDR & (1 << 11)) == 0);
			
		}// end if down pressed

		if (dif == 0)
		{
			printText("", 10, 20, RGBToWord(0,0,0), 0);
			printText("Press UP or DOWN", 10, 40, RGBToWord(255,255,255), 0);
			printText("to change", 10, 50, RGBToWord(255,255,255), 0);
			printText("difficulty", 10, 60, RGBToWord(255,255,255), 0);
		}// end if (difficulty == 0)

		if (dif == 1)
		{	
			redOff();
			yellowOff();
			greenOn();
			printText("EASY            ", 10, 20, RGBToWord(000,0xff,0), 0);
			printText("Press LEFT   ", 10, 100, RGBToWord(255,0,0), 0);
			printText("to select", 10, 110, RGBToWord(255,0,0), 0);
			printText("difficulty", 10, 120, RGBToWord(255,0,0), 0);
		}// end if (difficulty == 1)

		if (dif == 2)
		{
			redOff();
			greenOff();
			yellowOn();
			printText("NORMAL          ", 10, 20, RGBToWord(0xff,0xff,0), 0);
			printText("Press LEFT   ", 10, 100, RGBToWord(255,0,0), 0);
			printText("to select", 10, 110, RGBToWord(255,0,0), 0);
			printText("difficulty", 10, 120, RGBToWord(255,0,0), 0);
		}// end if (difficulty == 2)

		if (dif == 3)
		{	
			greenOff();
			yellowOff();
			redOn();
			printText("HARD            ", 10, 20, RGBToWord(255,0,0), 0);
			printText("Press LEFT   ", 10, 100, RGBToWord(255,0,0), 0);
			printText("to select", 10, 110, RGBToWord(255,0,0), 0);
			printText("difficulty", 10, 120, RGBToWord(255,0,0), 0);
		}// end if (difficulty == 3)

		if (dif == 4)
		{
			
			printText("Play Theme Music", 10, 20, RGBToWord(0,0,255), 0);
			printText("Press RIGHT  ", 10, 100, RGBToWord(255,0,0), 0);
			printText("to play  ", 10, 110, RGBToWord(255,0,0), 0);
			printText("music     ", 10, 120, RGBToWord(255,0,0), 0);
			int waitingForSelection = 0;

			if (waitingForSelection == 0)
			{
				while ((GPIOB->IDR & (1 << 4)) != 0) // Right button NOT pressed
				{
					// Green turns on, then off
					greenOn();
					delay(150);  // Wait for 150ms
					greenOff();
					delay(150);  // Wait for 150ms
					
					// Yellow turns on, then off
					yellowOn();
					delay(150);  // Wait for 150ms
					yellowOff();
					delay(150);  // Wait for 150ms
					
					// Red turns on, then off
					redOn(); 
					delay(150);  // Wait for 150ms
					redOff();
					delay(150);  // Wait for 150ms

					if ((GPIOA->IDR & (1 << 11))==0)  // Check if down button is pressed
					{
						break;
					} // end if down button is pressed

				}// while right button ISNT pressed

				if ((GPIOB->IDR & (1 << 4)) == 0)  // Check if right button is pressed
				{
					int themePlayed = 0;
					while (themePlayed == 0)
					{
						waitingForSelection = 1;
						initSound();
						playPVZTheme();
						themePlayed = 1;
					}// end while theme isnt played
				}// end while right button is pressed

			}// end while (waitingForSelection == 0)
		}// end while (difficulty == 4)

	}// end while (menu == 1)

	fillRectangle(0,0,128,160,0); // blacks out screen to clear it
	
	return dif; // returns difficulty chosen to the main

}// end int mainmenu(void)

// --------------------------------------------------------------------------------- TIMING & CLOCK

void initSysTick(void)
{
	SysTick->LOAD = 48000 - 1;
	SysTick->CTRL = 7;
	SysTick->VAL = 10;
	__asm(" cpsie i "); // enable interrupts
}

void SysTick_Handler(void)
{
	milliseconds++;
}

void delay(uint32_t dly)
{
	uint32_t end_time = dly + milliseconds;
	while(milliseconds != end_time)
	{
		__asm(" wfi "); // sleep
	}
}

// --------------------------------------------------------------------------------- DELAYS FOR THEMES

// Get the current time in milliseconds
uint32_t millis(void) 
{
    return milliseconds;
}

// allows notes to have a certain duration
void playNoteWithDuration(uint32_t freq, int duration)
{
	playNote(freq);             // Start playing the note
    noteStartTime = millis();   // Store the start time
    noteDuration = duration;    // Store the duration
}// end playNoteWithDuration

//
void updateNotePlayback() 
{
    if (millis() - noteStartTime >= noteDuration) 
	{
        TIM14->CR1 &= ~(1 << 0); // Stop sound after note
    }
}

// --------------------------------------------------------------------------------- THEME MUSIC

void playWinTheme(void)
{
	playNoteWithDuration(G3, 300);
    while (millis() - noteStartTime < noteDuration) updateNotePlayback();

    playNoteWithDuration(A3, 300);
    while (millis() - noteStartTime < noteDuration) updateNotePlayback();

    playNoteWithDuration(B3, 300);
    while (millis() - noteStartTime < noteDuration) updateNotePlayback();

    playNoteWithDuration(C4, 300);
    while (millis() - noteStartTime < noteDuration) updateNotePlayback();

    playNoteWithDuration(B3, 300);
    while (millis() - noteStartTime < noteDuration) updateNotePlayback();

    playNoteWithDuration(C4, 300);
    while (millis() - noteStartTime < noteDuration) updateNotePlayback();

	playNoteWithDuration(NoNote, 155);	// NO NOTE (to avoid drone noise after function executes)
    while (millis() - noteStartTime < noteDuration) updateNotePlayback();

}// end playWinTheme()

void playLoseTheme(void)
{
	playNoteWithDuration(A3, 300);
    while (millis() - noteStartTime < noteDuration) updateNotePlayback();

    playNoteWithDuration(F3, 300);
    while (millis() - noteStartTime < noteDuration) updateNotePlayback();

    playNoteWithDuration(CS3_Db3, 300);
    while (millis() - noteStartTime < noteDuration) updateNotePlayback();

	playNoteWithDuration(NoNote, 155);	// NO NOTE (to avoid drone noise after function executes)
    while (millis() - noteStartTime < noteDuration) updateNotePlayback();

}// end playLoseTheme()

void playPVZTheme(void) 
{
	// AAAB structure
	// loops 3 times for A 
    for (int i = 0; i < 3; i++) 
	{ 
        playNoteWithDuration(CS5_Db5, 300); // D FLAT
        while (millis() - noteStartTime < noteDuration) updateNotePlayback();

        playNoteWithDuration(D5, 300); 		// D
        while (millis() - noteStartTime < noteDuration) updateNotePlayback();

        playNoteWithDuration(CS5_Db5, 300); // D FLAT
        while (millis() - noteStartTime < noteDuration) updateNotePlayback();

        playNoteWithDuration(D5, 300);		// D
        while (millis() - noteStartTime < noteDuration) updateNotePlayback();

        playNoteWithDuration(AS4_Bb4, 300);	// B FLAT
        while (millis() - noteStartTime < noteDuration) updateNotePlayback();

        playNoteWithDuration(G4, 300);		// G
        while (millis() - noteStartTime < noteDuration) updateNotePlayback();

        playNoteWithDuration(G4, 300);		// G
        while (millis() - noteStartTime < noteDuration) updateNotePlayback();

        playNoteWithDuration(NoNote, 155);	// NO NOTE
        while (millis() - noteStartTime < noteDuration) updateNotePlayback();

		playNoteWithDuration(AS4_Bb4, 300);	// B FLAT
        while (millis() - noteStartTime < noteDuration) updateNotePlayback();

        playNoteWithDuration(G4, 300);		// G
        while (millis() - noteStartTime < noteDuration) updateNotePlayback();

        playNoteWithDuration(G4, 300);		// G
        while (millis() - noteStartTime < noteDuration) updateNotePlayback();

		playNoteWithDuration(NoNote, 155);	// NO NOTE
        while (millis() - noteStartTime < noteDuration) updateNotePlayback();

		playNoteWithDuration(D5, 300);		// D
        while (millis() - noteStartTime < noteDuration) updateNotePlayback();

		playNoteWithDuration(G4, 300);		// G
        while (millis() - noteStartTime < noteDuration) updateNotePlayback();

        playNoteWithDuration(G4, 300);		// G
        while (millis() - noteStartTime < noteDuration) updateNotePlayback();

		playNoteWithDuration(NoNote, 155);	// NO NOTE
        while (millis() - noteStartTime < noteDuration) updateNotePlayback();
    } // end for (int i = 0; i < 3; i++) 

	
	// B plays once
    playNoteWithDuration(A4, 300);
    while (millis() - noteStartTime < noteDuration) updateNotePlayback();

    playNoteWithDuration(AS4_Bb4, 300);
    while (millis() - noteStartTime < noteDuration) updateNotePlayback();

    playNoteWithDuration(A4, 300);
    while (millis() - noteStartTime < noteDuration) updateNotePlayback();

    playNoteWithDuration(AS4_Bb4, 300);
    while (millis() - noteStartTime < noteDuration) updateNotePlayback();

    playNoteWithDuration(G4, 300);
    while (millis() - noteStartTime < noteDuration) updateNotePlayback();

    playNoteWithDuration(D4, 300);
    while (millis() - noteStartTime < noteDuration) updateNotePlayback();

    playNoteWithDuration(D4, 300);
    while (millis() - noteStartTime < noteDuration) updateNotePlayback();

    playNoteWithDuration(CS4_Db4, 300);
    while (millis() - noteStartTime < noteDuration) updateNotePlayback();

    playNoteWithDuration(D4, 300);
    while (millis() - noteStartTime < noteDuration) updateNotePlayback();

    playNoteWithDuration(G4, 300);
    while (millis() - noteStartTime < noteDuration) updateNotePlayback();

    playNoteWithDuration(AS4_Bb4, 300);
    while (millis() - noteStartTime < noteDuration) updateNotePlayback();

    playNoteWithDuration(CS5_Db5, 300);
    while (millis() - noteStartTime < noteDuration) updateNotePlayback();

    playNoteWithDuration(CS5_Db5, 300);
    while (millis() - noteStartTime < noteDuration) updateNotePlayback();

	playNoteWithDuration(NoNote, 155); // no note to avoid drone noise
    while (millis() - noteStartTime < noteDuration) updateNotePlayback();
	
	
}// end playPVZTheme()

// --------------------------------------------------------------------------------- INITIALISATION

void initClock(void)
{
// This is potentially a dangerous function as it could
// result in a system with an invalid clock signal - result: a stuck system
        // Set the PLL up
        // First ensure PLL is disabled
        RCC->CR &= ~(1u<<24);
        while( (RCC->CR & (1 <<25))); // wait for PLL ready to be cleared
        
// Warning here: if system clock is greater than 24MHz then wait-state(s) need to be
// inserted into Flash memory interface
				
        FLASH->ACR |= (1 << 0);
        FLASH->ACR &=~((1u << 2) | (1u<<1));
        // Turn on FLASH prefetch buffer
        FLASH->ACR |= (1 << 4);
        // set PLL multiplier to 12 (yielding 48MHz)
        RCC->CFGR &= ~((1u<<21) | (1u<<20) | (1u<<19) | (1u<<18));
        RCC->CFGR |= ((1<<21) | (1<<19) ); 

        // Need to limit ADC clock to below 14MHz so will change ADC prescaler to 4
        RCC->CFGR |= (1<<14);

        // and turn the PLL back on again
        RCC->CR |= (1<<24);        
        // set PLL as system clock source 
        RCC->CFGR |= (1<<1);
}

void enablePullUp(GPIO_TypeDef *Port, uint32_t BitNumber)
{
	Port->PUPDR = Port->PUPDR &~(3u << BitNumber*2); // clear pull-up resistor bits
	Port->PUPDR = Port->PUPDR | (1u << BitNumber*2); // set pull-up bit
}

void pinMode(GPIO_TypeDef *Port, uint32_t BitNumber, uint32_t Mode)
{
	uint32_t mode_value = Port->MODER;
	Mode = Mode << (2 * BitNumber);
	mode_value = mode_value & ~(3u << (BitNumber * 2));
	mode_value = mode_value | Mode;
	Port->MODER = mode_value;
}

void setupIO()
{
	RCC->AHBENR |= (1 << 18) + (1 << 17); // enable Ports A and B
	display_begin();
	pinMode(GPIOB,4,0);
	pinMode(GPIOB,5,0);
	pinMode(GPIOA,8,0);
	pinMode(GPIOA,11,0);
	pinMode(GPIOA,0,1); // red LED
	pinMode(GPIOA,1,1); // green LED
	pinMode(GPIOA,9,1); // yellow LED
	enablePullUp(GPIOB,4);
	enablePullUp(GPIOB,5);
	enablePullUp(GPIOA,11);
	enablePullUp(GPIOA,8);

}// end setupIO

// -------------------------------------------------------------------- Hit box
int isInside(uint16_t x1, uint16_t y1, uint16_t w, uint16_t h, uint16_t px, uint16_t py, uint16_t padding)
{
    // Expand the hitbox in all directions
    x1 -= padding;  // Expand left
    y1 -= padding;  // Expand up
    w += padding * 2; // Expand width both ways
    h += padding * 2; // Expand height both ways

    uint16_t x2 = x1 + w - 1;  // Adjusted to match actual bounding box
    uint16_t y2 = y1 + h - 1;
    
    return (px >= x1 && px <= x2) && (py >= y1 && py <= y2);
}// end int isInside()

// -------------------------------------------------------------------- LED

// -------------- RED
void redOn()
{
    GPIOA->ODR |= (1 << 1);
}
void redOff()
{
    GPIOA->ODR &= ~(1 << 1);
}

// -------------- YELLOW
void yellowOn()
{
    GPIOA->ODR |= (1 << 9);
}
void yellowOff()
{
    GPIOA->ODR &= ~(1 << 9);
}

// -------------- GREEN
void greenOn()
{
    GPIOA->ODR |= (1 << 0);
}
void greenOff()
{
    GPIOA->ODR &= ~(1 << 0);
}

// -------------------------------------------------------------------- Check player's health

void checkhealth(int playerhealth,uint16_t x,uint16_t y,int flashhappened)
{
	// turns off certain lights if player loses health 
	if (playerhealth == 2)
	{
		putImage(x,y,16,16,pea2hp,0,0);
		greenOff();
		yellowOn();
		redOn();
	}// end if (playerhealth == 2)
	else if (playerhealth == 1)
	{
		putImage(x,y,16,16,pea1hp,0,0);
		greenOff();
		yellowOff();
	}// end else if (playerhealth == 1)
	else
	{
		// nothing
	}

}// end int checkhealth(int playerhealth,uint16_t x,uint16_t y,int flashhappened)

// -------------------------------------------------------------------- Randomiser

uint32_t randomise(uint32_t lower, uint32_t upper)
{
	return (prbs()%(upper-lower))+lower;
}

// -------------------------------------------------------------------- PRBS Generator

uint32_t prbs()
{
	// This is an unverified 31 bit PRBS generator
	// It should be maximum length but this has not been verified 
	unsigned long new_bit=0;	

    new_bit= ((shift_register & (1<<27))>>27) ^ ((shift_register & (1<<30))>>30);
    new_bit= ~new_bit;
    new_bit = new_bit & 1;
    shift_register=shift_register << 1;
    shift_register=shift_register | (new_bit);
		
	return shift_register & 0x7fffffff; // return 31 LSB's 
}
