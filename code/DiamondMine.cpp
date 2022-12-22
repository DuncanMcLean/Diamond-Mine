// Created by Duncan McLean 2022

#include <iostream>
#include <sstream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

using namespace sf;
using namespace std;

// Function declaration
void updateRocks(int seed);

const int NUM_ROCKS = 6;
Sprite rocks[NUM_ROCKS];

// Location of the player/rock
enum class side { LEFT, RIGHT, MIDDLE, NONE };
side rockPositions[NUM_ROCKS];

int main() {

	//Gets the screen width and height for later use
	int screenWidth = VideoMode::getDesktopMode().width;
	int screenHeight = VideoMode::getDesktopMode().height;

	VideoMode vm(screenWidth, screenHeight);

	// Creates the window, sets the size, and names the window
	RenderWindow window(vm, "Diamond Mine!", Style::Fullscreen);

	// Create a texture to hold te background graphic
	Texture textureBackground;

	// Load a graphic into the texture
	textureBackground.loadFromFile("graphics/minebackground.jpg");

	// Creates a background sprite
	Sprite spriteBackground;

	// Attach the texture to the sprite
	spriteBackground.setTexture(textureBackground);

	// Set the spriteBackground to cover the screen
	spriteBackground.setPosition(0, 0);

	// Prepare the diamond
	Texture textureDiamond;
	textureDiamond.loadFromFile("graphics/diamond.png");
	Sprite spriteDiamond;
	spriteDiamond.setTexture(textureDiamond);
	spriteDiamond.setPosition(0, 800);
	spriteDiamond.setScale(0.02f, 0.02f);

	// Hold value if diamond is on screen
	bool diamondActive = false;

	// Speed diamond fly
	float diamondSpeedX = 0.0f;
	float diamondSpeedY = 0.0f;

	// Spider texture
	Texture textureSpider;

	// Load spider texture
	textureSpider.loadFromFile("graphics/spider.png");

	// 3 spider sprites, sets the scale to fit the page
	Sprite spriteSpider1;
	Sprite spriteSpider2;
	Sprite spriteSpider3;
	spriteSpider1.setTexture(textureSpider);
	spriteSpider2.setTexture(textureSpider);
	spriteSpider3.setTexture(textureSpider);
	spriteSpider1.setScale(0.2f, 0.2f);
	spriteSpider2.setScale(0.2f, 0.2f);
	spriteSpider3.setScale(0.2f, 0.2f);

	// Position the spiders off screen
	spriteSpider1.setPosition(0, 0);
	spriteSpider2.setPosition(0, 150);
	spriteSpider3.setPosition(0, 300);

	// Holds value if spiders are on screen
	bool spider1Active = false;
	bool spider2Active = false;
	bool spider3Active = false;

	// Default speed of spiders
	float spider1Speed = 0.0f;
	float spider2Speed = 0.0f;
	float spider3Speed = 0.0f;

	// Variables to control time
	Clock clock;

	// Time bar setup
	RectangleShape timeBar;
	float timeBarStartWidth = 400;
	float timeBarHeight = 80;
	timeBar.setSize(Vector2f(timeBarStartWidth, timeBarHeight));
	timeBar.setFillColor(Color::Blue);
	timeBar.setPosition((screenWidth / 2) - timeBarStartWidth / 2, 980);

	Time gameTimeTotal;
	float timeRemaining = 6.0f;
	float timeBarWidthPerSecond = timeBarStartWidth / timeRemaining;

	// Track whether the game is running
	bool paused = true;

	// Score variable
	int score = 0;

	sf::Text messageText;
	sf::Text scoreText;

	// Loads font file
	sf::Font font;
	font.loadFromFile("fontFolder/open-sans/OpenSans-Light.ttf");

	// Set the font to our message
	messageText.setFont(font);
	scoreText.setFont(font);

	// Start menu and score
	messageText.setString("Press Enter to start!");
	scoreText.setString("Score = 0");
	messageText.setCharacterSize(75);
	scoreText.setCharacterSize(100);

	// Style text
	messageText.setFillColor(Color::Cyan);
	scoreText.setFillColor(Color::Cyan);

	// Positions the text
	FloatRect textRect = messageText.getLocalBounds();

	messageText.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);

	messageText.setPosition(1920 / 2.0f, 1080 / 2.0f);

	scoreText.setPosition(20, 20);

	// Prepares 5 rocks
	Texture textureRocks;
	textureRocks.loadFromFile("graphics/fallingrocks2.png");

	// Set the texture for each rock sprite
	for (int i = 0; i < NUM_ROCKS; i++) {
		rocks[i].setTexture(textureRocks);
		rocks[i].setPosition(-2000, -2000);
		rocks[i].setOrigin(220, 20);
		rocks[i].setScale(0.5f, 0.5f);
	}

	// Prepare the player
	Texture texturePlayer;
	texturePlayer.loadFromFile("graphics/miner.png");

	Sprite spritePlayer;
	spritePlayer.setTexture(texturePlayer);
	spritePlayer.setPosition(580, 720);
	spritePlayer.setScale(0.6f, 0.6f);

	// The player starts on the left
	side playerSide = side::LEFT;

	// Prepare the gravestone
	Texture textureRIP;
	textureRIP.loadFromFile("graphics/rip.png");

	Sprite spriteRIP;
	spriteRIP.setTexture(textureRIP);
	spriteRIP.setPosition(600, 860);
	spriteRIP.setScale(0.2f, 0.2f);

	// Prepare the pickaxe
	Texture texturePickaxe;
	texturePickaxe.loadFromFile("graphics/pickaxe.png");

	Sprite spritePickaxe;
	spritePickaxe.setTexture(texturePickaxe);
	spritePickaxe.setPosition(700, 830);
	spritePickaxe.setScale(0.1f, 0.1f);

	// Line the pickaxe up with the mined rock
	const float PICKAXE_POSITION_LEFT = 700;
	const float PICKAXE_POSITION_RIGHT = 1075;
	const float PICKAXE_POSITION_MIDDLE = 887;

	const float PICKAXE_Y_OTHER = 830;
	const float PICKAXE_Y_MIDDLE = 650;

	// Prepare the flying mined rock
	Texture textureMineRock;
	textureMineRock.loadFromFile("graphics/rockpile.png");

	Sprite spriteMineRock;
	spriteMineRock.setTexture(textureMineRock);
	spriteMineRock.setPosition(810, 720);
	spriteMineRock.setScale(0.3f, 0.3f);

	// Some other useful rock related variables
	bool rockActive = false;
	float rockSpeedX = 1000;
	float rockSpeedY = -1500;

	// Control the player input
	bool acceptInput = false;

	// Pickaxe hit sound
	SoundBuffer pickBuffer;
	pickBuffer.loadFromFile("sound/pickhit.wav");
	Sound pick;
	pick.setBuffer(pickBuffer);

	// Game over sound
	SoundBuffer deathBuffer;
	deathBuffer.loadFromFile("sound/gameover.wav");
	Sound death;
	death.setBuffer(deathBuffer);

	// Out of time
	SoundBuffer ootBuffer;
	ootBuffer.loadFromFile("sound/buzzer.wav");
	Sound outOfTime;
	outOfTime.setBuffer(ootBuffer);

	// Easy mode death counter
	int deathCounter = 0;

	// Main Game Loop
	while (window.isOpen()) {
		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == Event::KeyReleased && !paused) {
				// Listen for key presses again
				acceptInput = true;

				// hide the axe
				spritePickaxe.setPosition(2000, spritePickaxe.getPosition().y);
			}

		}

		/*
		****************************************
		Handle the players input
		****************************************
		*/

		if (Keyboard::isKeyPressed(Keyboard::Escape))
		{
			window.close();
		}

		// Start the game
		if (Keyboard::isKeyPressed(Keyboard::Return))
		{
			paused = false;

			// Reset the time and the score
			score = 0;
			timeRemaining = 6;

			// Make all the branches disappear
			for (int i = 1; i < NUM_ROCKS; i++)
			{
				rockPositions[i] = side::NONE;
			}

			// Make sure the gravestone is hidden
			spriteRIP.setPosition(675, 2000);

			// Move the player into position
			spritePlayer.setPosition(580, 720);

			acceptInput = true;

		}

		// Wrap the player controls to accept input
		if (acceptInput)
		{
			// First handle pressing the right cursor key
			if (Keyboard::isKeyPressed(Keyboard::Right))
			{
				// Make sure the player is on the right
				playerSide = side::RIGHT;

				score++;

				// Add to the amount of time remaining, easy mode time option
				if (deathCounter > 3) {
					timeRemaining += (2 / score) + .20;
				}
				else {
					timeRemaining += (2 / score) + .15;
				}

				spritePickaxe.setPosition(PICKAXE_POSITION_RIGHT, PICKAXE_Y_OTHER);
				spritePlayer.setPosition(1250, 720);

				// Update the rocks
				updateRocks(score);

				// set the mined rock flying to the left
				spriteMineRock.setPosition(810, 720);
				rockSpeedX = -5000;
				rockSpeedY = -1500;
				rockActive = true;

				acceptInput = false;

				// Play a chop sound
				pick.play();

			}

			// Handle the left cursor key
			if (Keyboard::isKeyPressed(Keyboard::Left))
			{
				// Make sure the player is on the left
				playerSide = side::LEFT;

				score++;

				// Add to the amount of time remaining, easy mode option available
				if (deathCounter > 3) {
					timeRemaining += (2 / score) + .20;
				}
				else {
					timeRemaining += (2 / score) + .15;
				}

				spritePickaxe.setPosition(PICKAXE_POSITION_LEFT, PICKAXE_Y_OTHER);

				spritePlayer.setPosition(480, 720);

				// Update the rocks
				updateRocks(score);

				// set the rock flying
				spriteMineRock.setPosition(810, 720);
				rockSpeedX = 5000;
				rockSpeedY = -1500;
				rockActive = true;

				acceptInput = false;

				// Play a chop sound
				pick.play();

			}

			// Handle the up cursor key
			if (Keyboard::isKeyPressed(Keyboard::Up))
			{
				// Make sure the player is on the middle
				playerSide = side::MIDDLE;

				score++;

				// Add to the amount of time remaining
				if (deathCounter > 3) {
					timeRemaining += (2 / score) + .20;
				}
				else {
					timeRemaining += (2 / score) + .15;
				}

				spritePickaxe.setPosition(PICKAXE_POSITION_MIDDLE, PICKAXE_Y_MIDDLE);

				spritePlayer.setPosition(840, 720);

				// Update the rocks
				updateRocks(score);

				// set the rock flying
				spriteMineRock.setPosition(940, 720);
				rockSpeedX = 0;
				rockSpeedY = -5000;
				rockActive = true;

				acceptInput = false;

				// Play a chop sound
				pick.play();

			}

		}

		/*
		****************************************
		Update the scene
		****************************************
		*/
		if (!paused)
		{
			// Measure time
			Time dt = clock.restart();

			// Subtract from the amount of time remaining
			timeRemaining -= dt.asSeconds();
			// size up the time bar
			timeBar.setSize(Vector2f(timeBarWidthPerSecond *
				timeRemaining, timeBarHeight));

			if (timeRemaining <= 0.0f) {

				// Pause the game
				paused = true;

				// Change the message shown to the player
				messageText.setString("Out of time!!");

				//Reposition the text based on its new size
				FloatRect textRect = messageText.getLocalBounds();
				messageText.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);

				messageText.setPosition(screenWidth / 2.0f, screenHeight / 2.0f);

				// Play the out of time sound
				outOfTime.play();

			}

			// Setup the diamond
			if (!diamondActive)
			{

				// How fast is the diamond
				diamondSpeedY = 500;
				diamondSpeedX = 4000;

				//Sets diamond position
				spriteDiamond.setPosition(900, 820);
				diamondActive = true;

			}
			else {
				// Move the diamond
				if (score % 2 == 0 && score != 0)
					spriteDiamond.setPosition(spriteDiamond.getPosition().x + (diamondSpeedX * dt.asSeconds()) * -1, spriteDiamond.getPosition().y + (diamondSpeedY * dt.asSeconds()));
				else
					diamondActive = false;

			}

			// Manage the spider
			// Spider 1
			if (!spider1Active)
			{
				// How fast is the spider
				srand((int)time(0) * 10);
				spider1Speed = (500);

				// How high is the spider
				srand((int)time(0) * 10);
				float width = (rand() % screenWidth);
				spriteSpider1.setPosition(width, -200);
				spider1Active = true;

			}
			else
			{
				spriteSpider1.setPosition(spriteSpider1.getPosition().x, spriteSpider1.getPosition().y + (spider1Speed * dt.asSeconds()));

				// Has the spider reached the right hand edge of the screen?
				if (spriteSpider1.getPosition().x > 1920 || spriteSpider1.getPosition().y > 1080)
				{
					// Set it up ready to be a whole new spider next frame
					spider1Active = false;
				}
			}
			// Spider 2
			if (!spider2Active)
			{
				// How fast is the spider
				srand((int)time(0) * 20);
				spider2Speed = ((rand() % 200) + 100);

				// How high is the spider
				srand((int)time(0) * 20);
				float height = (rand() % 300) - 150;
				spriteSpider2.setPosition(screenWidth, height);
				spider2Active = true;
			}
			else
			{

				spriteSpider2.setPosition(spriteSpider2.getPosition().x + (spider2Speed * dt.asSeconds() * -1), spriteSpider2.getPosition().y + (spider2Speed * dt.asSeconds()));

				// Has the spider reached the right hand edge of the screen?
				if (spriteSpider2.getPosition().x > screenWidth || spriteSpider2.getPosition().y > screenHeight)
				{
					// Set it up ready to be a whole new spider next frame
					spider2Active = false;
				}
			}

			if (!spider3Active)
			{

				// How fast is the spider
				srand((int)time(0) * 20);
				spider3Speed = ((rand() % 200) + 100);

				// How high is the spider
				srand((int)time(0) * 20);
				float height = (rand() % 600) - 150;
				spriteSpider3.setPosition(0, height);
				spider3Active = true;

			}
			else
			{
				spriteSpider3.setPosition(spriteSpider3.getPosition().x + (spider3Speed * dt.asSeconds()), spriteSpider3.getPosition().y + (spider3Speed * dt.asSeconds()));

				// Has the spider reached the right hand edge of the screen?
				if (spriteSpider3.getPosition().x > screenWidth || spriteSpider3.getPosition().y > screenHeight)
				{
					// Set it up ready to be a whole new spider next frame
					spider3Active = false;
				}
			}

			// Update the score text
			stringstream ss;
			ss << "Score = " << score;
			scoreText.setString(ss.str());

			// Update the rock sprites
			for (int i = 0; i < NUM_ROCKS; i++)
			{
				float height = i * 150;

				if (rockPositions[i] == side::LEFT)
				{
					// Move the sprite to the left side
					rocks[i].setPosition(610, height);
					// Flip the sprite round the other way
					rocks[i].setOrigin(220, 40);
					rocks[i].setRotation(1);
				}
				else if (rockPositions[i] == side::RIGHT)
				{
					// Move the sprite to the right side
					rocks[i].setPosition(1330, height);
					// Set the sprite rotation to normal
					rocks[i].setOrigin(220, 40);
					rocks[i].setRotation(1);

				}
				else if (rockPositions[i] == side::MIDDLE)
				{
					// Move the sprite to the right side
					rocks[i].setPosition(900, height);
					// Set the sprite rotation to normal
					rocks[i].setOrigin(220, 40);
					rocks[i].setRotation(0);

				}
				else
				{
					// Hide the rock
					rocks[i].setPosition(3000, height);
				}
			}

			// Handle a flying rock			
			if (rockActive)
			{
				spriteMineRock.setPosition(spriteMineRock.getPosition().x + (rockSpeedX * dt.asSeconds()), spriteMineRock.getPosition().y + (rockSpeedY * dt.asSeconds()));

				// Has the rock reached the right hand edge of the screen?
				if (spriteMineRock.getPosition().x < -100 || spriteMineRock.getPosition().x > 2000 || spriteMineRock.getPosition().y < -150)
				{
					// Set it up ready to be a whole new rock next frame
					rockActive = false;
					spriteMineRock.setPosition(810, 720);
				}
			}

			// Has the player been squished by a rock?
			if (rockPositions[5] == playerSide)
			{
				// Death
				paused = true;
				acceptInput = false;

				// If player dies under 5 seconds then the easy mode death counter increases
				if (dt.asSeconds() < 5)
					deathCounter++;

				// Draw the gravestone
				spriteRIP.setPosition(525, 760);

				// Hide the player
				spritePlayer.setPosition(2000, 660);

				// Change the text of the message
				messageText.setString("SQUANCHED!!");

				// Center it on the screen
				FloatRect textRect = messageText.getLocalBounds();

				messageText.setOrigin(textRect.left +
					textRect.width / 2.0f,
					textRect.top + textRect.height / 2.0f);

				messageText.setPosition(screenWidth / 2.0f, screenHeight / 2.0f);

				// Play the death sound
				death.play();

			}

		}// End if(!paused)

		 /*
		 ****************************************
		 Draw the scene
		 ****************************************
		 */

		 // Clear everything from the last frame
		window.clear();

		// Draw our game scene here
		window.draw(spriteBackground);

		// Draw the spiders
		window.draw(spriteSpider1);
		window.draw(spriteSpider2);
		window.draw(spriteSpider3);

		// Draw the falling rocks
		for (int i = 0; i < NUM_ROCKS; i++) {
			window.draw(rocks[i]);
		}

		// Draw the axe
		window.draw(spritePickaxe);

		// Draw the diamond
		window.draw(spriteDiamond);

		// Draw the flying mined rock
		window.draw(spriteMineRock);

		// Draw the player
		window.draw(spritePlayer);

		// Draw the gravestone
		window.draw(spriteRIP);

		// Draw the score
		window.draw(scoreText);

		// Draw the timebar
		window.draw(timeBar);

		if (paused)
		{
			// Draw our message
			window.draw(messageText);
		}

		// Show everything we just drew
		window.display();

	}//while window.isOpen

	return 0;

}//main

// Function definition
void updateRocks(int seed)
{
	// Move all the branches down one place
	for (int j = NUM_ROCKS - 1; j > 0; j--) {
		rockPositions[j] = rockPositions[j - 1];
	}

	// Spawn a new branch at position 0
	// LEFT, RIGHT or NONE
	srand((int)time(0) + seed);
	int r = (rand() % 5);

	switch (r) {
	case 0:
		rockPositions[0] = side::LEFT;
		break;

	case 1:
		rockPositions[0] = side::RIGHT;
		break;

	case 2:
		rockPositions[0] = side::MIDDLE;
		break;

	default:
		rockPositions[0] = side::NONE;
		break;
	}

}// updateRocks()