void drawDiceHistory()
{
	const int font_height = 8;
	const int font_width = 5;
	const int vertical_spacing = 1;
	const int horizontal_spacing = 3;
	const int total_x = 108;
	int x = 0, y = 0;
	u8g.setFont(SMALL_FONT);
	u8g.setFontPosTop();
	for (int roll = 0; roll < rolls_amount; roll++)
	{
		x = 0;
		int total = 0;
		if (!rolls[roll][0] == 0)// if the current roll isn't empty
		{
			for (int dice = 0; dice < MAX_DICE; dice++)
			{
				if (!rolls[roll][dice] == 0)// if the current dice isn't empty
				{
					total += rolls[roll][dice];
					u8g.drawStr(x, y, String(rolls[roll][dice]).c_str());
					x += (font_width * 2) + horizontal_spacing;
				}
			}
			String totalString = "=" + String(total);

			u8g.drawStr(total_x, y, totalString.c_str());
			y += font_height + vertical_spacing;
		} 
	}
}

void addNewRollToHistory()
{
	//if max reached, roll them leftward

	if (rolls_amount >= MAX_ROLLS_HISTORY)
	{
		//change current line to next line
		for (int roll = 0; roll < MAX_ROLLS_HISTORY; roll++)
		{
			for (int dice = 0; dice < MAX_DICE; dice++)
			{
				if (roll < MAX_ROLLS_HISTORY - 1)
				{
					rolls[roll][dice] = rolls[roll + 1][dice]; // roll+1 specifies next line
				} else
				{
					rolls[roll][dice] = 0;
				}
			}
		}
		rolls_amount--;
	}

	//add new roll
	randomSeed(analogRead(RANDOM_PIN) * millis());
	//Serial.print("New Roll. #");
	//Serial.print(rolls_amount);
	//Serial.print(": ");

	for (int roll = 0; roll < diceAmount; roll++)
	{
		int result = random(1, diceSelection+1);
		rolls[rolls_amount][roll] = result;
		//Serial.print(result);
		//Serial.print(", ");
	}
	//Serial.print(" /=");
	//Serial.println(getLastRollResult());

	rolls_amount++;

}

void clearRollHistory()
{
	for (int roll = 0; roll < MAX_ROLLS_HISTORY; roll++)
	{
		for (int dice = 0; dice < MAX_DICE; dice++)
		{
			rolls[roll][dice] = 0;
		}
	}
	rolls_amount = 0;
	Serial.println("Rolls Cleared");
}

int getLastRollResult()
{
	int result = 0;
	for (int dice = 0; dice < MAX_DICE; dice++)
	{
		result += rolls[rolls_amount-1][dice];
	}
	return result;
}

void drawRollResult()
{
	u8g.setFontPosCenter();
	u8g.setFont(LARGE_FONT);

	u8g.drawStr(WIDTH / 2 - 10, HEIGHT / 2, String(getLastRollResult()).c_str());

	//mini history
	u8g.setFont(SMALL_FONT);
	const int font_height = 8;
	const int font_width = 5;
	const int vertical_spacing = 1;
	const int horizontal_spacing = 3;
	const int total_x = 108;
	int x = 0, y = 45;
	int roll = 0;
	int total = 0;
	for (int dice = 0; dice < MAX_DICE; dice++)
	{
		if (!rolls[roll][dice] == 0)// if the current dice isn't empty
		{
			total += rolls[roll][dice];
			u8g.drawStr(x, y, String(rolls[roll][dice]).c_str());
			x += (font_width * 2) + horizontal_spacing;
		}
	}
	String totalString = "=" + String(total);

	u8g.drawStr(total_x, y, totalString.c_str());
	y += font_height + vertical_spacing;
}

void drawIndividualDiceRolls()
{

}