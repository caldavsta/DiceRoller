//DICE CONFIGS
int dice[] = { 2, 4, 6, 8, 10, 12, 20 };
int dice_length = sizeof(dice) / sizeof(dice[0]);

void drawDiceSelection()
{
	u8g.setFontPosCenter();
	u8g.setFont(SMALL_FONT);
	auto x = 0, y = 0;
	const auto DICE_SELECT_HEIGHT = 11;
	//const int CURSOR_HEIGHT = 6;

	//display the cursor under dice values
	//float topCursorX = getKnobValue(KNOB_LEFT) / 1024.0 * WIDTH;
	//u8g.drawVLine(topCursorX, DICE_SELECT_HEIGHT, CURSOR_HEIGHT);

	float selectedDice = getKnobValue(KNOB_LEFT) / 1024.0 * dice_length;

	for (int i = 0; i < dice_length; i++)
	{
		if (i == static_cast<int>(selectedDice))
		{
			u8g.setDrawColor(1);
			u8g.drawBox(x, y, WIDTH / dice_length, DICE_SELECT_HEIGHT);
			u8g.setDrawColor(0);
		} else
		{
			u8g.setDrawColor(1);
			u8g.drawFrame(x, y, WIDTH / dice_length, DICE_SELECT_HEIGHT);

		}

		u8g.drawStr((WIDTH/dice_length) * i + 3, y + DICE_SELECT_HEIGHT / 2 + 4, String(dice[i]).c_str());
		x += WIDTH / dice_length;
		 
	}
	u8g.setDrawColor(1);

}

void updateDiceSelectionFromPot()
{
	//calculate which of the selection is selected
	float selectedDice = getKnobValue(KNOB_LEFT) / 1024.0 * dice_length;

	//update global variable
	diceSelection = dice[static_cast<int>(selectedDice)];

	//for playing the sound when selection changes
	static int lastSelection = diceSelection;
	if (lastSelection != diceSelection)
	{
		tone(SPEAKER_PIN, 3729, 30);
		lastSelection = diceSelection;
	}
}

void updateDiceAmountFromPot()
{
	float selectedDiceAmount = getKnobValue(KNOB_RIGHT) / 1024.0 * MAX_DICE + 1.0;
	diceAmount = selectedDiceAmount;

	//for playing the sound when selection changes
	static int lastSelection = diceAmount;
	if (lastSelection != diceAmount)
	{
		tone(SPEAKER_PIN, 3729, 30);
		lastSelection = diceAmount;
	}
}

void drawDiceAmount()
{

	u8g.setFontPosCenter();
	u8g.setFont(LARGE_FONT);

	String diceString = "x" + String(diceAmount);
	u8g.drawStr(WIDTH/2+13, HEIGHT / 2 + 6, diceString.c_str());

	
}

void drawDiceText()
{

	u8g.setFontPosCenter();
	u8g.setFont(LARGE_FONT);
	String diceString = "D" + String(diceSelection);
	//Serial.println(diceString);
	u8g.drawStr(-2, HEIGHT/2+11, diceString.c_str());

}