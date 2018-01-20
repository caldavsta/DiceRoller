#include <MemoryFree.h>
#include "U8g2lib.h"
#include "SPI.h"
#include "Wire.h"
#include "Pitches.h"

//#include "fontdseg.c"

// STUFF FOR MY HARDWARE
#define BUTTON_LEFT 5
#define BUTTON_RIGHT 6
#define BUTTON_DEBUG 12
#define KNOB_LEFT 2
#define KNOB_RIGHT 1
#define KNOB_NOISE_THRESHOLD 15
#define RANDOM_PIN 0
#define SPEAKER_PIN 8
#define RESET_PIN 11
#define WIDTH 128
#define HEIGHT 64
// END STUFF FOR MY HARDWARE

#define SCREEN_DEBUG 3 
#define SCREEN_DICE_SELECT 0 
#define SCREEN_ROLL_RESULT 1
#define SCREEN_ROLL_HISTORY 2
#define MAX_DICE 8
#define MAX_ROLLS_HISTORY 6



#define LARGE_FONT u8g2_font_logisoso30_tr
#define SMALL_FONT u8g_font_5x8

//U8GLIB_SH1106_128X64 u8g(U8G_I2C_OPT_NONE);	// I2C 128x64(col2-col129) SH1106,Like HeiTec 1.3' I2C OLED 

// STUFF FOR MY HARDWARE
U8G2_SH1106_128X64_NONAME_2_HW_I2C u8g(U8G2_R0);

// buttons
int buttonPins[3] = { BUTTON_LEFT, BUTTON_RIGHT, BUTTON_DEBUG };
bool lastButtonState[3] = { false, false, false };
bool currentButtonState[3] = { false, false, false };

// knobs 
int knobStates[] = { 0,0 };
int knobPins[2] = { KNOB_LEFT, KNOB_RIGHT };
// END STUFF FOR MY HARDWARE


// application logic
int diceSelection = 0;
int diceAmount = 0;
int currentScreen = SCREEN_DICE_SELECT;

// roll storage
int rolls[MAX_ROLLS_HISTORY][MAX_DICE];
int rolls_amount = 0;

//debugging
bool occasionalDebug = false;
float debugFrequency = 0;



// FUNCTION DECLARATIONS
void drawMenuButtons(String left, String right, int leftOffset, int rightOffset);
void setButtonStates();
void setKnobStates();
void clearRollHistory();
bool getButtonDown(int arduinoPin);
int getKnobValue(int arduinoPin);
void drawDiceSelection();
void addNewRollToHistory();
void updateDiceSelectionFromPot();
void updateDiceAmountFromPot();
void drawDiceAmount();
void drawDiceText();
void drawDiceHistory();
void drawRollResult();
int getLastRollResult();
void playMelody(int* melody, int* noteDurations);
void draw();
void(*resetFunc) (void) = 0;
void outputAverageFrameRate();


void setup()
{
	//RESET PIN
	digitalWrite(RESET_PIN, HIGH);       // turn on pullup resistors
	pinMode(RESET_PIN, OUTPUT);           // set pin to input
	//setup arduino buttons
	pinMode(BUTTON_LEFT, INPUT);           // set pin to input
	digitalWrite(BUTTON_LEFT, HIGH);       // turn on pullup resistors
	pinMode(BUTTON_RIGHT, INPUT);           // set pin to input
	digitalWrite(BUTTON_RIGHT, HIGH);       // turn on pullup resistors
	pinMode(BUTTON_DEBUG, INPUT);           // set pin to input
	digitalWrite(BUTTON_DEBUG, HIGH);       // turn on pullup resistors
	pinMode(13, OUTPUT);
	digitalWrite(13, HIGH);

	Serial.begin(9600);
	Serial.print("Started Sketch. ");
	Serial.print(" Free Mem: ");
	Serial.println(freeMemory());
	clearRollHistory();

	u8g.begin();
	//set default draw color
	u8g.setDrawColor(1);

	tone(SPEAKER_PIN, 2230, 40);
}

void loop()
{
	//get vars for each loop
	setKnobStates();
	setButtonStates();

	// draw delayer tester
	const unsigned long fiveMinutes = 0.1 * 1000UL;
	static unsigned long lastSampleTime = 0 - fiveMinutes;  // initialize such that a reading is due the first time through loop()

	unsigned long now = millis();
	if (now - lastSampleTime >= fiveMinutes)
	{
		lastSampleTime += fiveMinutes;

		//draw();
	}

	bool leftButton = getButtonDown(BUTTON_LEFT);
	bool rightButton = getButtonDown(BUTTON_RIGHT);
	bool debugButton = getButtonDown(BUTTON_DEBUG);

	switch (currentScreen)
	{
	case SCREEN_DICE_SELECT: // dice selection
		updateDiceSelectionFromPot();
		updateDiceAmountFromPot();
		if (leftButton)
		{
			currentScreen = SCREEN_ROLL_HISTORY;

		}
		if (rightButton)
		{
			addNewRollToHistory();
			currentScreen = SCREEN_ROLL_RESULT;
			playMelody(rollMelody, rollDurations);
		}
		if (debugButton)
		{
			currentScreen = SCREEN_DEBUG;
		}
		break;
	case SCREEN_ROLL_RESULT:
		static bool played = false;
		if (diceSelection == 20 && !played)
		{ 
				if (getLastRollResult() == 20)
				{
					playMelody(winMelody, winNoteDurations);
					played = true;
				}
				if (getLastRollResult() == 1)
				{
					playMelody(loseMelody, loseNoteDurations);
					played = true;
				}
		}

		if (leftButton)
		{
			currentScreen = SCREEN_ROLL_HISTORY;
			played = false;
		}
		if (rightButton)
		{
			currentScreen = SCREEN_DICE_SELECT;
			played = false;
		}
		break;
	case SCREEN_ROLL_HISTORY:
		if (leftButton)
		{
			clearRollHistory();
		}
		if (rightButton)
		{
			currentScreen = SCREEN_DICE_SELECT;
		}
		break;
	case SCREEN_DEBUG:
		debugFrequency = getKnobValue(KNOB_RIGHT) / 1024.0 * 4978.0;
		if (getButtonDown(BUTTON_LEFT))
		{
			tone(SPEAKER_PIN, debugFrequency, 40);
		}
		if (getButtonDown(BUTTON_RIGHT))
		{
			currentScreen = SCREEN_DICE_SELECT;
		}
		break;
	}

	//outputAverageFrameRate();

	draw();
	delay(1);
}

void outputAverageFrameRate()
{
	long now = millis();
	static long lastMillis = now - 100;

	long timeSince = now - lastMillis;
	lastMillis = now;

	if(timeSince < 10)
	{
		//u8g.begin();
		Serial.print("Time is ");
		Serial.print(timeSince);
		Serial.println(" Resetting Display");
		Serial.print(" Free Mem: ");
		Serial.println(freeMemory());
		playMelody(errorMelody, errorNoteDurations);

		//u8g.sleepOff();
		//resetFunc();
		//analogWrite(RESET_PIN, LOW);
		delay(20000);
	}
	Serial.println(timeSince);

}

void draw()
{
	u8g.firstPage();
	do
	{
		switch (currentScreen)
		{
		case SCREEN_DICE_SELECT: // dice selection			
			drawMenuButtons("History", "Roll", 10, 20);
			drawDiceText();
			drawDiceAmount();
			drawDiceSelection();
			break;
		case SCREEN_ROLL_RESULT:
			drawRollResult();
			drawMenuButtons("History", "Back", 10, 10);
			break;
		case SCREEN_ROLL_HISTORY:
			drawDiceHistory();
			drawMenuButtons("Clear", "Back", 7, 5);
			break;
		case SCREEN_DEBUG:
			u8g.setFontPosTop();
			u8g.drawStr(0,0,String(debugFrequency).c_str());
			u8g.drawStr(0, 20, String(freeMemory()).c_str());
			drawMenuButtons("Play", "Back", 10, 10);
			break;
	
	}
	} while (u8g.nextPage());
}

void setKnobStates()
{
	for (int i = 0; i < sizeof(knobStates) / sizeof(knobStates[0]); i++)
	{
		int newState = 1023 - analogRead(knobPins[i]); //invert knob reading
		if (abs(newState - knobStates[i]) > KNOB_NOISE_THRESHOLD)
		{
			knobStates[i] = newState;
		}
	}
}

void setButtonStates()
{
	for (int i = 0; i < sizeof(currentButtonState) / sizeof(currentButtonState[0]); i++)
	{
		lastButtonState[i] = currentButtonState[i];
		if (digitalRead(buttonPins[i]) == LOW)
		{
			currentButtonState[i] = false;
		} else
		{
			currentButtonState[i] = true;
		}
	}
}

int getKnobValue(int arduinoPin)
{
	int knobToCheck = -1;
	switch (arduinoPin)
	{
	case KNOB_LEFT:
		knobToCheck = 0;
		break;
	case KNOB_RIGHT:
		knobToCheck = 1;
		break;
	}
	return knobStates[knobToCheck];
}

bool getButtonDown(int arduinoPin)//button is the actual button number on arduino
{
	int buttonToCheck = -1;
	switch (arduinoPin)
	{
	case BUTTON_LEFT:
		buttonToCheck = 0;
		break;
	case BUTTON_RIGHT:
		buttonToCheck = 1;
		break;
	case BUTTON_DEBUG:
		buttonToCheck = 2;
		break;

	}

	if (currentButtonState[buttonToCheck] == false && lastButtonState[buttonToCheck] == true)
	{
		//tone(SPEAKER_PIN, 2230, 40);
		playMelody(selectMelody, selectDurations);
		return true;
	} else
	{
		return false;
	}
}

void drawMenuButtons(String left, String right, int leftOffset, int rightOffset)
{
	const int MARGIN = 3;
	const int BUTTON_HEIGHT = 10;
	const int BUTTON_SPACING = 5; //doubled


	u8g.drawRFrame(MARGIN            , HEIGHT - BUTTON_HEIGHT - 1, WIDTH / 2 - MARGIN * 2, BUTTON_HEIGHT * 2, 5);
	u8g.drawRFrame(WIDTH / 2 + MARGIN, HEIGHT - BUTTON_HEIGHT - 1, WIDTH / 2 - MARGIN * 2, BUTTON_HEIGHT * 2, 5);
	//u8g.drawFrame(, , WIDTH / 2, BUTTON_HEIGHT);

	//draw text
	u8g.setFontPosBottom();
	u8g.setFont(SMALL_FONT);

	u8g.drawStr(MARGIN + leftOffset, HEIGHT-1, left.c_str());
	u8g.drawStr(WIDTH / 2 + MARGIN + rightOffset, HEIGHT-1, right.c_str());
}

