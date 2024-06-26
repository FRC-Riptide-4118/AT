// AVR header, needed for all registers, etc.
#include <avr/io.h>

// input defines
#define BUZZ_CTL 2
#define LED_CTL  3
#define SENSE    0
// output defines
#define BUZZ     6
#define LED0     4
#define LED1     5
#define PRESS    1

// this is a really stupid way to do a crappy buzzer
// ideally, we would generate a nice waveform with hardware
// but I just output a square wave and filter it
#define BUZZ_FREQ_CTL 3300

// helpful macros
#define READ_PIN(reg, pin) reg.IN  & (1 << pin)
#define SET_PIN(reg, pin)  reg.OUTSET |= (1 << pin)
#define CLR_PIN(reg, pin)  reg.OUTCLR |= (1 << pin)
#define TGL_PIN(reg, pin)  reg.OUTTGL |= (1 << pin)

// global volatile variables
volatile uint16_t buzz_cnt;
volatile uint8_t  buzz_flag;

// user-defined functions
void io_config(void);
void run_button_func(void);
void run_LED_func(void);
void run_buzz_func(void);

// main entry point
// do initialization, and then loop through tasks
int main(void)
{
	// INITIALIZE
	io_config();
	
	// LOOP
	while(1) {
		
		// this one is most important, does button functionality
		// everything else can be as slow or fast as we want
		run_button_func();
		// control LEDs
		run_LED_func();
		// control buzzer
		run_buzz_func();
	
	}
    
}

// set up all of our IO stuff
void io_config(void) {
	
	// set specific pins to be outputs
	// note that other pins are inputs, which is the default
	PORTB.DIRSET |= (1 << PRESS);
	PORTB.DIRSET |= (1 << BUZZ) | (1 << LED0) | (1 << LED1);
	
	// let's set the internal pull-up just in case on the SENSE line
	// we can also invert it to make the logic cleaner
	// going to do the same with the LED_CTL and BUZZ_CTL inputs
	PORTB.PIN0CTRL |= PORT_PULLUPEN_bm | PORT_INVEN_bm;
	PORTB.PIN2CTRL |= PORT_PULLUPEN_bm | PORT_INVEN_bm;
	PORTB.PIN3CTRL |= PORT_PULLUPEN_bm | PORT_INVEN_bm;
	
}

// primary button functionality
// translates the capacitive touch > actual button press
void run_button_func(void) {
	
	if (READ_PIN(PORTB, SENSE)) {
		SET_PIN(PORTB, PRESS);
	} else {
		CLR_PIN(PORTB, PRESS);
	}
	
}

// turn the LEDs on when a touch is detected
// only if LED_CTL is true too
void run_LED_func(void) {
	
	if (READ_PIN(PORTB, SENSE) && READ_PIN(PORTB, LED_CTL)) {
		SET_PIN(PORTB, LED0);
		SET_PIN(PORTB, LED1);
	} else {
		CLR_PIN(PORTB, LED0);
		CLR_PIN(PORTB, LED1);
	}
	
}

// control the buzzer
// note: if the tone is super horrible, there are better ways to do this
// currently outputting a crappy square wave at ~1KHz and filtering it down to a tone
void run_buzz_func(void) {
	
	if (READ_PIN(PORTB, SENSE) && READ_PIN(PORTB, BUZZ_CTL)) {
		buzz_cnt++;
		if (buzz_cnt == BUZZ_FREQ_CTL) {
			buzz_cnt = 0;
			TGL_PIN(PORTA, BUZZ);
		}
			
	} else {
		CLR_PIN(PORTA, BUZZ);
	}
	
}