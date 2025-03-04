#include <stm32f031x6.h>
#include "display.h"
#include "sound.h"
#include "musical_notes.h"
#include "font5x7.h"
#include "stm32f0xx.h"  // Ensure you include the STM32 header
void initClock(void);
void initSysTick(void);
void SysTick_Handler(void);
void delay(volatile uint32_t dly);
void setupIO();
int isInside(uint16_t x1, uint16_t y1, uint16_t w, uint16_t h, uint16_t px, uint16_t py, uint16_t padding);
void enablePullUp(GPIO_TypeDef *Port, uint32_t BitNumber);
void pinMode(GPIO_TypeDef *Port, uint32_t BitNumber, uint32_t Mode);
void playWinTheme();
void playPVZTheme();
volatile uint32_t milliseconds;

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

int main()
{
	int vinverted = 0;
	int toggle = 0;
	int vmoved = 0;
	int tempx = 0;

	uint16_t x = 20;
	uint16_t y = 80;
	uint16_t oldx = x;
	uint16_t oldy = y;
    uint16_t padding = 5; // padding around hitbox 

	initClock();
	initSysTick();
	setupIO();
	putImage(20,80,16,16,shooter,0,0); //displays player starting position

	float zombie1X = 100; // starting x position of zombie1
	float zombie1Y = 20; // starting y position of zombie1

	float zombie2X = 100; // starting x position of zombie1
	float zombie2Y = 60; // starting y position of zombie1

	float zombie3X = 100; // starting x position of zombie1
	float zombie3Y = 100; // starting y position of zombie1

	float zombie4X = 100; // starting x position of zombie2
	float zombie4Y = 140; // starting y position of zombie2

	while(1)
	{
		zombie1X = zombie1X - 0.3; // Move zombie1 to the left by 0.3 pixel per frame
		zombie2X = zombie2X - 0.3; // Move zombie1 to the left by 0.3 pixel per frame
		zombie3X = zombie3X - 0.3; // Move zombie1 to the left by 0.3 pixel per frame
    	zombie4X = zombie4X - 0.3; // Move zombie2 to the left by 0.3 pixel per frame

		putImage(zombie1X, zombie1Y, 16, 16, zombie, 0, 0);
		putImage(zombie2X, zombie2Y, 16, 16, zombie, 0, 0);
		putImage(zombie3X, zombie3Y, 16, 16, zombie, 0, 0);
    	putImage(zombie4X, zombie4Y, 16, 16, zombie, 0, 0);

        int zombieDead = 0;

		if (zombie1X < 0) 
        {
			putImage(zombie1X,zombie1Y,16,16,black,0,0);
            zombieDead = zombieDead + 1;
			//zombie1X = 100; // Reset zombie to start position if it moves off the screen
		}
		if (zombie2X < 0) {
			putImage(zombie2X,zombie2Y,16,16,black,0,0);
            zombieDead = zombieDead + 1;
			//zombie2X = 100; // Reset zombie to start position if it moves off the screen
		}
		if (zombie3X < 0) {
			putImage(zombie3X,zombie3Y,16,16,black,0,0);
            zombieDead = zombieDead + 1;
			//zombie3X = 100; // Reset zombie to start position if it moves off the screen
		}
		if (zombie4X < 0) {
			putImage(zombie4X,zombie4Y,16,16,black,0,0);
            zombieDead = zombieDead + 1;
			//zombie4X = 100; // Reset zombie to start position if it moves off the screen
		}

        if (zombieDead == 4)
        {
            initSound();
			//playWinTheme();  || zombie2X || zombie3X || zombie4X 
			printTextX2("YOU WIN!", 10, 20, RGBToWord(0xff,0xff,0), 0);
            
        }
		
		if (zombie1X < 20)
		{
			initSound();
			//playLoseTheme();
			printTextX2("GAMEOVER!", 10, 20, RGBToWord(0xff,0xff,0), 0);
		}
		
		vmoved = 0; //resets if player moved so it doesnt stay in loop
		// --------------------------------------------------------------------------------- IF RIGHT PRESSED
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

				if (isInside(zombie1X, zombie1Y, 16, 16, x, y, 5)) //if in zombie lane1
				{
					putImage(zombie1X, zombie1Y, 16, 16, black, 0, 0); //places black over zombie start -> make into zombies current location
                    zombie1X = -100;
                    break; // stops pellet moving after it hits zombie
				}
				if (isInside(zombie2X, zombie2Y, 16, 16, x, y, 5)) //if in zombie lane2
				{
					putImage(zombie2X, zombie2Y, 16, 16, black, 0, 0);
                    zombie2X = -100;
                    break;
				}
                if (isInside(zombie3X, zombie3Y, 16, 16, x, y, 5)) //if in zombie lane2
				{
					putImage(zombie3X, zombie3Y, 16, 16, black, 0, 0);
                    zombie3X = -100;
                    break;
				}
                if (isInside(zombie4X, zombie4Y, 16, 16, x, y, 5)) //if in zombie lane2
				{
					putImage(zombie4X, zombie4Y, 16, 16, black, 0, 0);
                    zombie4X = -100;
                    break;
				}
				
				

			}// end while(x<100)

			x = tempx; //restores x position for player	

		}// end if right is pressed

		// --------------------------------------------------------------------------------- IF LEFT PRESSED
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
		// --------------------------------------------------------------------------------- IF DOWN PRESSED
		if ( (GPIOA->IDR & (1 << 11)) == 0) // down pressed
		{
			if (y < 140)
			{
				y = y + 3;			
				vmoved = 1;
			}// end inner if

		}// end if
		// --------------------------------------------------------------------------------- IF UP PRESSED
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
	SysTick->LOAD = 48000;
	SysTick->CTRL = 7;
	SysTick->VAL = 10;
	__asm(" cpsie i "); // enable interrupts
}

void SysTick_Handler(void)
{
	milliseconds++;
}

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

void delay(volatile uint32_t dly)
{
	uint32_t end_time = dly + milliseconds;
	while(milliseconds != end_time)
		__asm(" wfi "); // sleep
}


// --------------------------------------------------------------------------------- THEME MUSIC

void playNoteWithDuration(uint32_t freq, int duration) 
{
	playNote(freq);  // Set frequency
	delay_ms(duration); // Set duration
	TIM14->CR1 &= ~(1 << 0); // Stop sound after note
	delay_ms(50); // Short silence between notes

}// end playNoteWithDuration

void delay_ms(uint32_t ms) {
    SysTick->LOAD = (48000 * ms) - 1;  // Assuming 48MHz clock, 1ms delay
    SysTick->VAL = 0;                  // Reset the counter
    SysTick->CTRL = 5;                  // Enable SysTick, use system clock

    while ((SysTick->CTRL & 0x10000) == 0);  // Wait for the COUNTFLAG

    SysTick->CTRL = 0;  // Disable SysTick
}// end delay_ms
	
void playWinTheme(void)
{
    playNoteWithDuration(G3, 300);
    playNoteWithDuration(A3, 300);
    playNoteWithDuration(B3, 300);
    playNoteWithDuration(C4, 300);
    playNoteWithDuration(B3, 300);
    playNoteWithDuration(C4, 300);
}
void playLoseTheme(void)
{
    playNoteWithDuration(A3, 300);
    playNoteWithDuration(F3, 300);
    playNoteWithDuration(CS3_Db3, 300);
}
void playPvZTheme(void) 
{
	// -------------------------------------------------------- A 
	int i = 0;
	for (i = 0; i < 3; i++) // plays this part 3 times 
	{
		playNoteWithDuration(CS5_Db5, 300); 	// D flat
		playNoteWithDuration(D5, 300); 			// D
		playNoteWithDuration(CS5_Db5, 300); 	// D flat
		playNoteWithDuration(D5, 300); 			// D

		playNoteWithDuration(AS4_Bb4, 300); 	// B flat
		playNoteWithDuration(G4, 300);			// G
		playNoteWithDuration(G4, 300);			// G
		playNoteWithDuration(NoNote, 155);		// delay

		playNoteWithDuration(AS4_Bb4, 300);		// B flat
		playNoteWithDuration(G4, 300);			// G
		playNoteWithDuration(G4, 300);			// G
		playNoteWithDuration(NoNote, 155);		// delay
		
		playNoteWithDuration(D5, 300);			// D
		playNoteWithDuration(G4, 300);			// G
		playNoteWithDuration(G4, 300);			// G
		playNoteWithDuration(NoNote, 155);		// delay

	}// end for
	// -------------------------------------------------------- B

	playNoteWithDuration(A4, 300); 				// A
	playNoteWithDuration(AS4_Bb4, 300);			// B flat
	playNoteWithDuration(A4, 300); 				// A
	playNoteWithDuration(AS4_Bb4, 300); 		// B flat

	playNoteWithDuration(G4, 300); 				// G
	playNoteWithDuration(D4, 300);				// D
	playNoteWithDuration(D4, 300);				// D
	playNoteWithDuration(CS4_Db4, 300);			// D flat
	playNoteWithDuration(D4, 300);				// D

	playNoteWithDuration(G4, 300);				// G
	playNoteWithDuration(AS4_Bb4, 300);			// B flat
	playNoteWithDuration(CS5_Db5, 300);			// D flat
	playNoteWithDuration(CS5_Db5, 300);			// D flat
		
	
}// end playPVZTheme

// --------------------------------------------------------------------------------- INITIALISATION

void enablePullUp(GPIO_TypeDef *Port, uint32_t BitNumber)
{
	Port->PUPDR = Port->PUPDR &~(3u << BitNumber*2); // clear pull-up resistor bits
	Port->PUPDR = Port->PUPDR | (1u << BitNumber*2); // set pull-up bit
}
void pinMode(GPIO_TypeDef *Port, uint32_t BitNumber, uint32_t Mode)
{
	/*
	*/
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
	enablePullUp(GPIOB,4);
	enablePullUp(GPIOB,5);
	enablePullUp(GPIOA,11);
	enablePullUp(GPIOA,8);

}// end setupIO
