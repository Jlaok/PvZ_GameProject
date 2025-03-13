#include <stm32f031x6.h>
#include <stdlib.h>
#include "display.h"
#include "sound.h"
#include "musical_notes.h"
#include "font5x7.h"
#include "stm32f0xx.h"  // Ensure you include the STM32 header
#include <stdint.h>
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
void playWinTheme();
void playPVZTheme(void);
void updateNotePlayback();
void testsound();
//void delay_ms(uint32_t ms);
void playNoteWithDuration(uint32_t freq, int duration);
void greenOn(void);
void greenOff(void);
void redOn(void);
void redOff(void);
void yellowOn(void);
void yellowOff(void);
uint32_t randomise(uint32_t,uint32_t);
uint32_t millis(void);
void updateNotePlayback();
volatile uint32_t milliseconds;
uint32_t noteStartTime = 0;
int noteDuration = 0;
int mainmenu(int);


const uint16_t black[]= //black 16x16
{
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

const uint16_t pellet[]= //pellet shot
{
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,9293,9293,0,0,0,0,0,0,0,0,0,0,0,0,0,9293,54559,54559,9293,0,0,0,0,0,0,0,0,0,0,0,0,9293,54559,54559,9293,0,0,0,0,0,0,0,0,0,0,0,0,0,9293,9293,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

const uint16_t zombie[]= //zombie
{
	0,0,0,51266,0,58674,58674,58674,58674,58674,17441,17441,0,0,0,0,0,0,0,0,51266,51803,53124,53124,53124,53124,53124,53124,17441,0,51266,0,0,0,0,58674,51803,53124,53124,53124,53124,58674,58674,58674,53124,51266,0,0,0,0,51266,51266,58674,58674,53124,53124,53124,24527,24527,24527,53124,58674,0,0,0,0,0,58674,49448,24527,53124,53124,53124,24527,49448,24527,53124,51266,51266,0,0,0,0,58674,24527,24527,53124,53124,53124,24527,24527,24527,19035,58674,0,0,0,0,0,58674,19035,19035,53124,53124,53124,19035,19035,19035,53124,58674,0,0,0,0,0,0,0,65535,2584,65535,2584,53124,53124,53124,17441,0,0,0,0,0,0,0,0,2584,2584,2584,2584,2584,53124,58674,58674,0,0,0,0,0,0,0,0,65535,2584,2584,65535,2584,53124,58674,0,0,0,0,0,0,0,0,0,34064,17441,17441,17441,17441,58674,34064,0,0,0,0,0,0,0,0,2073,2073,34064,65535,4912,65535,34064,2073,2073,0,0,0,0,0,53124,2073,2073,0,34064,65535,4912,65535,34064,0,2073,0,0,0,0,0,0,0,0,0,42305,34658,34658,42305,42305,0,53124,0,0,0,0,0,0,0,0,42305,34658,0,0,0,53124,0,0,0,0,0,0,0,0,0,34064,34064,0,0,0,34064,34064,0,0,0,0,0,
};

const uint16_t shooter[]= //shooter
{
	0,0,9293,0,0,0,0,0,0,0,0,0,0,0,0,0,0,9293,9293,0,0,54559,54559,54559,54559,0,0,0,0,0,0,0,9293,9293,9293,9293,11542,54559,54559,54559,54559,54559,0,0,0,0,0,0,0,9293,9293,11542,54559,54559,54559,54559,54559,54559,54559,0,0,0,9293,0,0,9293,0,11542,54559,54559,54559,54559,54559,0,54559,54559,54559,9293,54559,9293,0,9293,0,11542,54559,54559,54559,54559,54559,54559,54559,54559,54559,9293,54559,9293,0,0,0,11542,54559,54559,54559,54559,54559,54559,54559,0,0,0,9293,0,0,0,0,0,11542,54559,54559,54559,54559,54559,0,0,0,0,0,0,0,0,0,0,0,11542,54559,54559,54559,0,0,0,0,0,0,0,0,0,0,0,0,0,11542,11542,0,0,0,0,0,0,0,0,0,0,9293,0,9293,0,11542,54559,0,9293,0,9293,0,0,0,0,0,0,0,9293,9293,0,11542,54559,0,9293,9293,0,0,0,0,0,0,0,0,9293,9293,0,11542,54559,0,9293,9293,0,0,0,0,0,0,0,0,0,9293,9293,11542,54559,9293,9293,0,0,0,0,0,0,0,0,0,0,0,9293,9293,9293,9293,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};
const uint16_t deco2[]= //flowervar2
{
	0,0,9293,0,0,0,0,0,0,0,0,0,0,0,0,0,0,9293,9293,0,0,54559,54559,54559,54559,0,0,0,0,0,0,0,9293,9293,9293,9293,11542,54559,54559,54559,54559,54559,0,0,0,0,0,0,0,9293,9293,11542,54559,54559,54559,54559,54559,54559,54559,0,0,0,9293,0,0,9293,0,11542,54559,54559,54559,54559,54559,0,54559,54559,54559,9293,54559,9293,0,9293,0,11542,54559,54559,54559,54559,54559,54559,54559,54559,54559,9293,54559,9293,0,0,0,11542,54559,54559,54559,54559,54559,54559,54559,0,0,0,9293,0,0,0,0,0,11542,54559,54559,54559,54559,54559,0,0,0,0,0,0,0,0,0,0,0,11542,54559,54559,54559,0,0,0,0,0,0,0,0,0,0,0,0,0,11542,11542,0,0,0,0,0,0,0,0,0,0,9293,0,9293,0,11542,54559,0,9293,0,9293,0,0,0,0,0,0,0,9293,9293,0,11542,54559,0,9293,9293,0,0,0,0,0,0,0,0,9293,9293,0,11542,54559,0,9293,9293,0,0,0,0,0,0,0,0,0,9293,9293,11542,54559,9293,9293,0,0,0,0,0,0,0,0,0,0,0,9293,9293,9293,9293,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

 	uint16_t background[]=//background
{
	0,0,0,0,0,0,0,0,0,0,0,0,0
};

const uint16_t grass[]=//grass background
{
	266,65535,65535,266,65535,9293,65535,65535,65535,65535,65535,9293,65535,266,65535,9293,9293,65535,65535,9293,65535,65535,65535,266,65535,65535,9293,65535,65535,65535,65535,65535,65535,65535,9293,65535,65535,9293,65535,9293,65535,65535,65535,65535,65535,9293,266,65535,65535,65535,9293,65535,65535,9293,65535,65535,9293,65535,266,65535,9293,65535,65535,65535,266,65535,65535,65535,266,65535,65535,65535,9293,65535,9293,65535,266,65535,65535,65535,9293,65535,65535,65535,9293,65535,65535,65535,65535,65535,9293,65535,9293,65535,65535,65535,65535,9293,65535,65535,65535,65535,9293,65535,65535,65535,65535,65535,65535,65535,9293,65535,65535,65535,65535,9293,65535,65535,266,9293,65535,65535,266,65535,65535,65535,65535,9293,65535,65535,9293,65535,65535,65535,65535,9293,65535,65535,9293,9293,65535,65535,65535,266,65535,65535,266,65535,65535,65535,65535,65535,65535,65535,65535,9293,65535,65535,65535,65535,65535,65535,65535,65535,65535,9293,65535,65535,266,65535,65535,65535,65535,65535,65535,65535,266,9293,65535,65535,9293,65535,65535,65535,9293,65535,9293,65535,65535,65535,266,65535,65535,65535,9293,65535,266,65535,65535,9293,9293,65535,65535,9293,65535,65535,9293,9293,65535,65535,9293,65535,65535,65535,65535,266,65535,65535,65535,65535,65535,65535,65535,9293,65535,65535,65535,65535,9293,65535,65535,65535,65535,65535,65535,9293,65535,9293,65535,65535,65535,266,65535,65535,65535,9293,65535,65535,65535,65535,65535,266,65535,65535,9293,65535,
};

int main()
{
	int toggle = 0;
	int vmoved = 0;
	int tempx = 0;
	int random = -1;
	int zombieDead = 0;
	int count = 0;
	int z1 = 0;
	int z2 = 0;
	int z3 = 0;
	int z4 = 0;
	int winThemePlayed = 0;
	int loseThemePlayed = 0;
	int PVZthemePlayed = 0;
	int difficulty = 0;
	int difmod = 1;

	uint16_t x = 20;
	uint16_t y = 80;
	uint16_t oldx = x;
	uint16_t oldy = y;
    uint16_t padding = 5; // padding around hitbox 

	initClock();
	initSysTick();
	setupIO();
	putImage(20,80,16,16,shooter,0,0); //displays player starting position
	greenOn();
	redOn();

	


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

	int play_theme = 1;



	difmod = mainmenu(difficulty);	//main menu at the start of the game
	



	while(1)
	{

		if (zombieDead == count)
		{
		random = randomise(1,6); // chooses a random zombie to send
		count++;
		}

		if (zombie1.zombnum == random)
		{
			z1 = z1 - 0.1; // Move zombie1 to the left by 0.3 pixel per frame
			putImage(z1, zombie1.zomby, 16, 16, zombie, 0, 0);

			if (z1 < 0) 
			{
				putImage(z1,zombie1.zomby,16,16,black,0,0);
				zombieDead++;
				z1 = zombie1.zombx;
				//zombie1X = 100; // Reset zombie to start position if it moves off the screen
			}
		}

		else if (zombie2.zombnum == random)
		{
			z2 = z2 - 0.1; // Move zombie1 to the left by 0.3 pixel per frame
			putImage(z2, zombie2.zomby, 16, 16, zombie, 0, 0);

			if (z2 < 0) 
			{
				putImage(z2,zombie2.zomby,16,16,black,0,0);
				zombieDead++;
				z2 = zombie2.zombx;
				//zombie1X = 100; // Reset zombie to start position if it moves off the screen
			}
		}

		else if (zombie3.zombnum == random)
		{
			z3 = z3 - 0.1; // Move zombie1 to the left by 0.3 pixel per frame
			putImage(z3, zombie3.zomby, 16, 16, zombie, 0, 0);

			if (z3 < 0) 
			{
				putImage(z3,zombie3.zomby,16,16,black,0,0);
				zombieDead++;
				z3 = zombie3.zombx;
				//zombie1X = 100; // Reset zombie to start position if it moves off the screen
			}
		}

		else if (zombie4.zombnum == random)
		{
			z4 = z4 - 0.1; // Move zombie1 to the left by 0.3 pixel per frame
			putImage(z4, zombie4.zomby, 16, 16, zombie, 0, 0);

			if (z4 < 0) 
			{
				putImage(z4,zombie4.zomby,16,16,black,0,0);
				zombieDead++;
				z4 = zombie4.zombx;
				//zombie1X = 100; // Reset zombie to start position if it moves off the screen
			}
		}
// --------------------------------------------------------------------------------------------------- WIN	
		// if 10 zombies killed, player wins
        if (zombieDead == 10)
        {
			if (winThemePlayed == 0) 
			{
				initSound(); 
				printTextX2("YOU WIN!", 10, 20, RGBToWord(0xff,0xff,0), 0);
				random = 100;
				putImage(zombie1.zombx, zombie1.zomby, 16, 16, black, 0, 0);
				putImage(zombie2.zombx, zombie2.zomby, 16, 16, black, 0, 0);
				putImage(zombie3.zombx, zombie3.zomby, 16, 16, black, 0, 0);
				putImage(zombie4.zombx, zombie4.zomby, 16, 16, black, 0, 0);
				playWinTheme();
				winThemePlayed = 1;  // Set the flag to indicate the win theme has been played
			}// end inner if

        }// end if
		
// --------------------------------------------------------------------------------------------------- LOSE
		// if ANY zombie has an x == 20, player loses
		if (z1 == 20 || z2 == 20 || z3 == 20 || z4 == 20)
		{
			if (loseThemePlayed == 0)
			{
				initSound();
				printTextX2("GAMEOVER!", 10, 20, RGBToWord(0xff,0xff,0), 0);
				playLoseTheme();
				loseThemePlayed = 1;
			}// end inner if 

		}// end if
/*
		if (z1 == 20)
		{
			if (loseThemePlayed == 0)
			{
				initSound();
				printTextX2("GAMEOVER!", 10, 20, RGBToWord(0xff,0xff,0), 0);
				playLoseTheme();
				loseThemePlayed = 1;
			}
		}
		else if (z2 == 20)
		{
			
		}
		else if (z3 == 20)
		{
			initSound();
			printTextX2("GAMEOVER!", 10, 20, RGBToWord(0xff,0xff,0), 0);
			playLoseTheme();
		}
		else if (z4 == 20)
		{
			initSound();
			printTextX2("GAMEOVER!", 10, 20, RGBToWord(0xff,0xff,0), 0);
			playLoseTheme();
		}
*/		
		vmoved = 0; //resets if player moved so it doesnt stay in loop

		// --------------------------------------------------------------------------------- IF RIGHT PRESSED > (SHOOTING)
		if ((GPIOB->IDR & (1 << 4))==0) // right pressed
		{	
			initSound();  // Initialize sound
			putImage((x+20),y,16,16,pellet,0,0); // puts pellet 20 spots away from peashooter to ensure wont make it disappear
			tempx = x; // stores x
			x = x+20; // moves pellet out

			while (x < 100) // while != isinside? so pellet is not going til end
			{
				x = x + 1;
				delay(20);
				putImage(x, y, 16, 16, pellet, 0, 0); // move pellet forward

				if (isInside(z1, zombie1.zomby, 16, 16, x, y, 5)) //if in zombie lane1
				{
					putImage(z1, zombie1.zomby, 16, 16, black, 0, 0); //places black over zombie start -> make into zombies current location
                    z1 = -100;
                    break; // stops pellet moving after it hits zombie
				}
				if (isInside(z2, zombie2.zomby, 16, 16, x, y, 5)) //if in zombie lane2
				{
					putImage(z2, zombie2.zomby, 16, 16, black, 0, 0);
                    z2 = -100;
                    break;
				}
                if (isInside(z3, zombie3.zomby, 16, 16, x, y, 5)) //if in zombie lane2
				{
					putImage(z3, zombie3.zomby, 16, 16, black, 0, 0);
                    z3 = -100;
                    break;
				}
                if (isInside(z4, zombie4.zomby, 16, 16, x, y, 5)) //if in zombie lane2
				{
					putImage(z4, zombie4.zomby, 16, 16, black, 0, 0);
                    z4 = -100;
                    break;
				}
				
				

			}// end while(x<100)

			x = tempx; //restores x position for player	

		}// end if right is pressed

		// --------------------------------------------------------------------------------- IF LEFT PRESSED <
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
		if ( (GPIOA->IDR & (1 << 11)) == 0) // down pressed
		{
			if (y < 140)
			{
				y = y + 3;			
				vmoved = 1;
			}// end inner if

		}// end if
		// --------------------------------------------------------------------------------- IF UP PRESSED ^
		if ( (GPIOA->IDR & (1 << 8)) == 0) // up pressed
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
				else
				{
					putImage(x,y,16,16,deco2,0,0);
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

	}// end while(1)

	return 0;

}// end main()



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

void playNoteWithDuration(uint32_t freq, int duration)
{
	playNote(freq);             // Start playing the note
    noteStartTime = millis();   // Store the start time
    noteDuration = duration;    // Store the duration
}// end playNoteWithDuration

void updateNotePlayback() 
{
    if (millis() - noteStartTime >= noteDuration) 
	{
        TIM14->CR1 &= ~(1 << 0); // Stop sound after note
    }
}

// delay_ms() doesnt allow the game to continue after the function is executed, hence why we used millis()
/*
void delay_ms(uint32_t ms)
{
    SysTick->LOAD = (48000 * ms) - 1;  // Assuming 48MHz clock, 1ms delay
    SysTick->VAL = 0;                  // Reset the counter
    SysTick->CTRL = 5;                  // Enable SysTick, use system clock

    while ((SysTick->CTRL & 0x10000) == 0);  // Wait for the COUNTFLAG

    SysTick->CTRL = 0;  // Disable SysTick
}// end delay_ms
*/

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
}// end playWinTheme()

void playLoseTheme(void)
{
	playNoteWithDuration(A3, 300);
    while (millis() - noteStartTime < noteDuration) updateNotePlayback();

    playNoteWithDuration(F3, 300);
    while (millis() - noteStartTime < noteDuration) updateNotePlayback();

    playNoteWithDuration(CS3_Db3, 300);
    while (millis() - noteStartTime < noteDuration) updateNotePlayback();
}// end playLoseTheme()

void playPVZTheme(void) 
{
	int i = 0;

	// AAAB structure

	// loops 3 times for A 
    for (i = 0; i < 3; i++) 
	{
        playNoteWithDuration(CS5_Db5, 300);
        while (millis() - noteStartTime < noteDuration) updateNotePlayback();

        playNoteWithDuration(D5, 300);
        while (millis() - noteStartTime < noteDuration) updateNotePlayback();

        playNoteWithDuration(CS5_Db5, 300);
        while (millis() - noteStartTime < noteDuration) updateNotePlayback();

        playNoteWithDuration(D5, 300);
        while (millis() - noteStartTime < noteDuration) updateNotePlayback();

        playNoteWithDuration(AS4_Bb4, 300);
        while (millis() - noteStartTime < noteDuration) updateNotePlayback();

        playNoteWithDuration(G4, 300);
        while (millis() - noteStartTime < noteDuration) updateNotePlayback();

        playNoteWithDuration(G4, 300);
        while (millis() - noteStartTime < noteDuration) updateNotePlayback();

        playNoteWithDuration(NoNote, 155);
        while (millis() - noteStartTime < noteDuration) updateNotePlayback();
    }

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
int isInside(uint16_t x1, uint16_t y1, uint16_t w, uint16_t h, uint16_t px, uint16_t py, uint16_t padding)
{
	// checks to see if point px,py is within the rectange defined by x,y,w,h
    /*
	uint16_t x2, y2;
	x2 = x1 + w;
	y2 = y1 + h;
	int rvalue = 0;
	if ( (px >= x1) && (px <= x2))
	{
		// ok, x constraint met
		if ( (py >= y1) && (py <= y2))
			rvalue = 1;
	}
	return rvalue;
    */
    // Expand the hitbox in all directions
    x1 -= padding;  // Expand left
    y1 -= padding;  // Expand up
    w += padding * 2; // Expand width both ways
    h += padding * 2; // Expand height both ways


    uint16_t x2 = x1 + w - 1;  // Adjusted to match actual bounding box
    uint16_t y2 = y1 + h - 1;
    
    return (px >= x1 && px <= x2) && (py >= y1 && py <= y2);
}

void setupIO()
{
	RCC->AHBENR |= (1 << 18) + (1 << 17); // enable Ports A and B
	display_begin();
	pinMode(GPIOB,4,0);
	pinMode(GPIOB,5,0);
	pinMode(GPIOA,8,0);
	pinMode(GPIOA,11,0);
	pinMode(GPIOA,0,1);
	pinMode(GPIOA,1,1);
	enablePullUp(GPIOB,4);
	enablePullUp(GPIOB,5);
	enablePullUp(GPIOA,11);
	enablePullUp(GPIOA,8);

}// end setupIO

// -------------------------------------------------------------------- LED

void redOn()
{
    GPIOA->ODR |= (1 << 1);
}
void redOff()
{
    GPIOA->ODR &= ~(1 << 1);
}
void greenOn()
{
    GPIOA->ODR |= (1 << 0);
}
void greenOff()
{
    GPIOA->ODR &= ~(1 << 0);
}
void yellowOn()
{
    GPIOA->ODR &= ~(1 << 2);
}
void yellowOff()
{
    GPIOA->ODR &= ~(1 << 2);
}

uint32_t randomise(uint32_t lower, uint32_t upper)
{
	return (prbs()%(upper-lower))+lower;
}
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


int mainmenu(int dif)
{	
	int menu = 1;//in meni

	

	while(menu == 1)
	{
		
		if ((GPIOB->IDR & (1 << 5))==0)//left is pressed
		{
			menu = 0;//exit menu
		}	

		if ((GPIOB->IDR & (1 << 4))==0) // right pressed
		{
			dif = dif + 1;

		}

		if (dif == 1)
		{	
			//redOff();
			//greenOn();
			printTextX2("Difficulty: ZOMBOSS", 10, 20, RGBToWord(000,000,0), 0);
			printTextX2("Difficulty: Sunflower", 10, 20, RGBToWord(000,0xff,0), 0);
			
		}

		else if (dif == 2)
		{
			//greenOff();
			//yellowOn();
			printTextX2("Difficulty: Sunflower", 10, 20, RGBToWord(000,000,0), 0);
			printTextX2("Difficulty: Crazy Dave", 10, 20, RGBToWord(0xff,0xff,0), 0);
		}
		else
		{
			//yellowOff();
			//redOn();
			printTextX2("Difficulty: Crazy Dave", 10, 20, RGBToWord(000,000,0), 0);
			printTextX2("Difficulty: ZOMBOSS", 10, 20, RGBToWord(0xff,0xff,0), 0);
		}
	}
	
	if (dif == 1 )
	{
		printTextX2("Difficulty: Sunflower", 10, 20, RGBToWord(000,000,0), 0);
	}

	else if (dif ==2)
	{
		printTextX2("Difficulty: Crazy Dave", 10, 20, RGBToWord(000,000,0), 0);
	}

	else 
	{
		printTextX2("Difficulty: ZOMBOSS", 10, 20, RGBToWord(000,000,0), 0);
	}
	
	return dif;
}

/*
void laygrass(void)
{
	int x = 0;
	int y = 0;
	int i;
	int j;

	for (j = 0; j < 10; j++)
		for (i = 0; i < 8; i++ )
			{
				putImage(x, y, 16, 16, grass, 0, 0);
				x = x + 16;
			}
		y = y +16;
*/







