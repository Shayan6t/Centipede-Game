#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

using namespace std;
using namespace sf;

// Initializing Dimensions.
// resolutionX and resolutionY determine the rendering resolution.
// Don't edit unless required. Use functions on lines 43, 44, 45 for resizing the game window.
const int resolutionX = 960;
const int resolutionY = 960;
const int boxPixelsX = 32;
const int boxPixelsY = 32;
const int gameRows = resolutionX / boxPixelsX; // Total rows on grid
const int gameColumns = resolutionY / boxPixelsY; // Total columns on grid
// Initializing GameGrid.
int gameGrid[gameRows][gameColumns] = {};
// The following exist purely for readability.
const int x = 0;
const int y = 1;
const int exists = 2;

///!!!FUNCTION DECLARATIONS!!!///

//For movement of player
void movePlayer(float player[],  Event& e);
//For the mushrooms at random position
void makingmushroom(float& sX, float& sY);
//Generate the random positions for mushrooms on the window
void generateMushrooms(float mushroom[][2], int& num);
//Draw the mushrooms at generated positions
void drawmushroom( RenderWindow& window, float mushroom[][2], Sprite& mushroomSprite, int num);
//Drawing the player
void drawPlayer( RenderWindow& window, float player[],  Sprite& playerSprite);
//Movement of bullet when space is pressed
void moveBullet(float bullet[],  Clock& bulletClock,  Sound& collisionSound, int& num, float mushroom[][2], Sprite& mushroomSprite,const int& numSegments, float centipede[][2], bool centipedeDirection[], int& score);
//Drawing the bullet
void drawBullet(RenderWindow& window, float bullet[],  Sprite& bulletSprite);
//For the detection of bullet and mushroom collision and removing mushroom
void BMCollision(float bullet[], float mushroom[][2], int num,  Sound& collisionSound, Sprite& mushroomSprite, int& score);
//For the movemnet of centipede
void moveCentipede(float centipede[][2], bool centipedeDirection[12], const int& numSegments,  Clock& centipedeClock, float mushroom[][2], int& num, float& centipedeSpeed);
//For drawing the centipede 
void drawCentipede( RenderWindow& window, float centipede[][2],  Sprite& headSprite, Sprite& segmentSprite, const int& numSegments);
//For the detection of bullet and centipede collision and removing the hit segment of centipede
void BCcollision(float bullet[], float centipede[][2], bool centipedeDirection[], const int& numSegments,  Sound& collisionSound, int& score);
//For the detection of centipede and player collision and ending the game
bool CPcollision(float player[], float centipede[][2], const int& numSegments);
//For the detection of centipede and mushroom collision and changing the direction of centipede accordingly
void CMcollision(float centipede[][2], bool direction[], const int& numSegments, float mushroom[][2], const int& numMushrooms);

int main()
{
	int score=0;
	srand(time(0));
	// Declaring RenderWindow.
	 RenderWindow window( VideoMode(resolutionX, resolutionY), "Centipede",  Style::Close |  Style::Titlebar);

	// Used to resize your window if it's too big or too small. Use according to your needs.
	window.setSize( Vector2u(640, 640)); // Recommended for 1366x768 (768p) displays.
	// window.setSize( Vector2u(1280, 1280)); // Recommended for 2560x1440 (1440p) displays.
	// window.setSize( Vector2u(1920, 1920)); // Recommended for 3840x2160 (4k) displays.

	// Used to position your window on every launch. Use according to your needs.
	window.setPosition( Vector2i(250, 0));

	// Initializing Background Music.
	Music bgMusic;
	bgMusic.openFromFile("Music/field_of_hopes.ogg");
	bgMusic.play();
	bgMusic.setVolume(50);

	// Initializing Background.
	Texture backgroundTexture;
	Sprite backgroundSprite;
	backgroundTexture.loadFromFile("Textures/nature.jpg");
	backgroundSprite.setTexture(backgroundTexture);
	backgroundSprite.setColor(Color(255, 255, 255, 255*0.5)); // Reduces Opacity to 25%

	
	Font fonts;
	fonts.loadFromFile("font/Rocker Squad.ttf"); // Load a font for displaying text
	Text scoreText("Score: 0", fonts, 20); // Initialize the score text
	scoreText.setPosition(10, resolutionY - 30); // Position the text at the bottom left corner
	scoreText.setFillColor(Color::White); // Set text color

	// Initializing Player and Player Sprites.
	float player[2] = {};
	player[x] = (gameColumns / 2) * boxPixelsX;
	player[y] = (gameColumns * 3 / 4) * boxPixelsY;
	Texture playerTexture;
	Sprite playerSprite;
	playerTexture.loadFromFile("Textures/player.png");
	playerSprite.setTexture(playerTexture);
	playerSprite.setTextureRect(IntRect(0, 0, boxPixelsX, boxPixelsY));

	// Initializing Bullet and Bullet Sprites.
	float bullet[3] = {};
	bullet[x] = player[x];
	bullet[y] = player[y] - boxPixelsY;
	bullet[exists] = true;
	Clock bulletClock;
	Texture bulletTexture;
	Sprite bulletSprite;
	bulletTexture.loadFromFile("Textures/bullet.png");
	bulletSprite.setTexture(bulletTexture);
	bulletSprite.setTextureRect( IntRect(0, 0, boxPixelsX, boxPixelsY));
	bool fireBullet = false; // Flag to track bullet firing
	SoundBuffer fireSoundBuffer;// Sound of fire
	fireSoundBuffer.loadFromFile("Sound Effects/newBeat.wav");
	Sound fireSound;
	fireSound.setBuffer(fireSoundBuffer);

	// Initializing mushroom and mushroom Sprites.
	Texture mushroomTexture;
	Sprite mushroomSprite;
	mushroomTexture.loadFromFile("Textures/mushroom.png");
	mushroomSprite.setTexture(mushroomTexture);
	mushroomSprite.setTextureRect(IntRect(0, 0, boxPixelsX, boxPixelsY));
	int  num = 20;
	float mushroom[20][2];
	generateMushrooms(mushroom, num);
	
	//Sound for bullet and mushroom strike
	SoundBuffer collisionSoundBuffer;
	collisionSoundBuffer.loadFromFile("Sound Effects/fire1.wav");
	Sound collisionSound;
	collisionSound.setBuffer(collisionSoundBuffer);

	//Working For Centipede
	const int numSegments = 12; // Number of segments in the centipede
	float centipede[numSegments][2]={}; // Centipede position array
	Texture headTexture, segmentTexture; // Textures for head and segments
	Sprite headSprite, segmentSprite; // Sprites for head and segments
	headSprite.setTextureRect(IntRect(0, 0, boxPixelsX, boxPixelsY));
	segmentSprite.setTextureRect(IntRect(0, 0, boxPixelsX, boxPixelsY));
	headTexture.loadFromFile("Textures/heads.png"); // Load head texture
	segmentTexture.loadFromFile("Textures/segment.png"); // Load segment texture
	headSprite.setTexture(headTexture); // Set head texture
	segmentSprite.setTexture(segmentTexture); // Set segment texture
	Clock centipedeClock;
	// Initialize centipede position
	centipede[0][x] = 0; // Starting from left edge
	centipede[0][y] = 0;
	for (int i=1;i<numSegments;++i) 
	{
		centipede[i][x]=centipede[i-1][x]+boxPixelsX;// Starting from left edge
		centipede[i][y]=0;// Starting from top edge
	}
	bool centipedeDirection[12]={ 1,1,1,1,1,1,1,1,1,1,1,1, }; // Initial direction of centipede movement
	float centipedeSpeed=70.0;
	
	// Loading font for the gameover display
	Font font;
        font.loadFromFile("font/Rocker Squad.ttf"); // Load a font for displaying text
        Text gameOverText("Game Over",font,100);
        gameOverText.setPosition(150,400);
        gameOverText.setFillColor(Color::Red);
        // Sound for player and centipede strike(GAME OVER)
        SoundBuffer gameoverSoundBuffer;
	gameoverSoundBuffer.loadFromFile("Sound Effects/flea.wav");
	Sound gameoverSound;
	gameoverSound.setBuffer(gameoverSoundBuffer);
	
	while (window.isOpen()) 
	{
		window.draw(backgroundSprite);
		drawmushroom(window, mushroom, mushroomSprite, num);
		drawPlayer(window, player, playerSprite);
		scoreText.setString("Score: " + to_string(score));
		window.draw(scoreText); // Draw the score text
		if (bullet[exists] == true) 
		{
			moveBullet(bullet, bulletClock, collisionSound, num, mushroom, mushroomSprite, numSegments, centipede, centipedeDirection, score);
			drawBullet(window, bullet, bulletSprite);
		}
		 Event e;
		while (window.pollEvent(e)) 
		{

			if (e.type ==  Event::Closed) 
			{
				return 0;
			}
			else if (e.type ==  Event::KeyPressed)
			{
				if (e.key.code ==  Keyboard::Space)
				{
				fireSound.play();
					if (!bullet[exists])
					{ //Check for the bullet if it not exist
						fireBullet = true; // Set the fireBullet flag to launch the bullet
						bullet[x] = player[x]; // Position the bullet at player's x-coordinate
						bullet[y] = player[y] - boxPixelsY; // Position the bullet above the player
						bullet[exists] = true; // Mark the bullet as existing
					}
				}
			}
			movePlayer(player, e);
		}
		moveCentipede(centipede, centipedeDirection, numSegments, centipedeClock, mushroom, num, centipedeSpeed); // Move the centipede
		CMcollision(centipede, centipedeDirection, numSegments, mushroom, num); // Check collision with mushrooms
		drawCentipede(window, centipede, headSprite, segmentSprite, numSegments); // Draw the centipede
		if (CPcollision(player, centipede, numSegments)) 
		{
			gameoverSound.play();
			window.draw(gameOverText);
        		window.display();
         		sleep(seconds(3)); // Display the message for 2 seconds
        		return 0; // End the game loop
    		}
		window.display();
		window.clear();
	}
}

void movePlayer(float player[],  Event& event) 
{
	float moveSpeed = 32; // Adjust movement speed as needed
	int bottomRows = gameRows - 6; // Bottom five rows
	if (event.type ==  Event::KeyPressed) 
	{
		if (event.key.code ==  Keyboard::Left && player[x] > 0) 
		{
			player[x] -= moveSpeed;
		}
		else if (event.key.code ==  Keyboard::Right && player[x] < (gameColumns - 1) * boxPixelsX) 
		{
			player[x] += moveSpeed;
		}
		else if (event.key.code ==  Keyboard::Up && player[y] > bottomRows * boxPixelsY) 
		{
			player[y] -= moveSpeed;
		}
		else if ((event.key.code ==  Keyboard::Down && player[y] < gameColumns * boxPixelsY) && player[y] < 960 - 32) 
		{
			player[y] += moveSpeed;
		}
	}
}

void makingmushroom(float& sX, float& sY) 
{
	sX =(rand() % ((gameColumns - 2) * boxPixelsX)); // Random x-coordinate
	sY =((rand() % (gameRows - 8)) * boxPixelsY);//Random y-coordinates(excluding bottom 8 rows)
}
void generateMushrooms(float mushroom[][2], int& num) 
{
	for (int i = 0; i < num; ++i) 
	{
		makingmushroom(mushroom[i][0], mushroom[i][1]); // Generate mushroom coordinates
	}
}
void drawmushroom( RenderWindow& window, float mushroom[][2],  Sprite& mushroomSprite, int num) 
{
	for (int i = 0; i < num; ++i) 
	{
		mushroomSprite.setPosition(mushroom[i][0], mushroom[i][1]); // Set the position
		window.draw(mushroomSprite); // Draw the mushroom sprite
	}
}
void drawPlayer( RenderWindow& window, float player[],  Sprite& playerSprite) 
{
	playerSprite.setPosition(player[x], player[y]);
	window.draw(playerSprite);
}
void moveBullet(float bullet[],  Clock& bulletClock,  Sound& collisionSound, int& num, float mushroom[][2], Sprite& mushroomSprite,const int& numSegments, float centipede[][2], bool centipedeDirection[], int& score) 
{
	if (bulletClock.getElapsedTime().asMilliseconds() < 10)
		return;
	bulletClock.restart();
	bullet[y] -= 10;
	BMCollision(bullet, mushroom, num, collisionSound, mushroomSprite, score); // Check collision with mushrooms
	BCcollision(bullet, centipede, centipedeDirection, numSegments, collisionSound, score);
	if (bullet[y] < -32)
		bullet[exists] = false;
}
void drawBullet( RenderWindow& window, float bullet[],  Sprite& bulletSprite) 
{
	bulletSprite.setPosition(bullet[x], bullet[y]);
	window.draw(bulletSprite);
}
void BMCollision(float bullet[], float mushroom[][2], int num,  Sound& collisionSound, Sprite& mushroomSprite, int&score) 
{
    	for (int i = 0; i < num; ++i) 
    	{
		if (bullet[x] >= mushroom[i][0] && bullet[x] <= mushroom[i][0] + boxPixelsX && bullet[y] >= mushroom[i][1] && bullet[y] <= mushroom[i][1] + boxPixelsY) 
		{
			mushroom[i][0] = -1000; // Move the mushroom outside the visible area
			mushroom[i][1] = -1000;
			bullet[exists] = false; // Destroy the bullet after collision
			collisionSound.play();
			score += 5;
			break; // Exit the loop after handling the collision
		}
	}
}

void moveCentipede(float centipede[][2], bool direction[], const int& numSegments,  Clock& centipedeClock, float mushroom[][2], int& numMushrooms, float& centipedeSpeed) 
{
    if (centipedeClock.getElapsedTime().asMilliseconds() < centipedeSpeed)
        return;
    centipedeClock.restart();
    const int bottomRows = gameRows - 5; // Define the bottom five rows
    for (int i = 0; i < numSegments; i++) 
    {
        // Check if the centipede segment is within the bottom five rows
        bool playerArea = centipede[i][y] >= bottomRows * boxPixelsY;
        if (direction[i]) 
        { // Moving right
            if (centipede[i][x] > 0) 
            {
                centipede[i][x] -= boxPixelsX;
            } 
            else if (playerArea) 
            {
                direction[i] = false;
                centipede[i][y] -= boxPixelsY;
            } 
            else 
            {
                direction[i] = false;
                centipede[i][y] += boxPixelsY;
            }
	} 
	else 
	{ // Moving left
            if (centipede[i][x] < 960 - boxPixelsX - 20) 
            {
                centipede[i][x] += boxPixelsX;
            }
            else if (playerArea) 
            {
                direction[i] = true;
                centipede[i][y] -= boxPixelsY;
            } 
            else 
            {
                direction[i] = true;
                centipede[i][y] += boxPixelsY;
            }
        }
    }
}
void drawCentipede( RenderWindow& window, float centipede[][2], Sprite& headSprite, Sprite& segmentSprite, const int& numSegments) 
{
	for(int i = 0; i < numSegments; ++i) 
	{
		if (i == 0) 
		{
			headSprite.setPosition(centipede[i][x], centipede[i][y]); // Head position
			window.draw(headSprite); // Draw head
		}
		else 
		{
			segmentSprite.setPosition(centipede[i][x], centipede[i][y]); // Segment positions
			window.draw(segmentSprite); // Draw segments
		}
	}
}
void CMcollision(float centipede[][2], bool direction[], const int& numSegments, float mushroom[][2], const int& numMushrooms) 
{
    for(int i = 0; i < numMushrooms; ++i) 
    {
        for(int j = 0; j < numSegments; ++j) 
        {
            if((centipede[j][x] >= mushroom[i][0]) && (centipede[j][x] <= mushroom[i][0] + boxPixelsX) && (centipede[j][y] >= mushroom[i][1]) && (centipede[j][y] <= mushroom[i][1] + boxPixelsY)) 
            {
                // Collision occurred with mushroom i

                // Move only the collided segment to the row below the one with the mushroom
                int mushroomRow = static_cast<int>(mushroom[i][1] / boxPixelsY); // Get the mushroom row
                centipede[j][y] = (mushroomRow + 1) * boxPixelsY; // Move the segment below the mushroom's row
                // Change direction for the collided segment based on its current direction
                if (direction[j]) 
                {
                    centipede[j][x] += boxPixelsX; // Move right
                } 
                else 
                {
                    centipede[j][x] -= boxPixelsX; // Move left
                }
                direction[j] = !direction[j]; // Invert the direction for the collided segment
                break; // Move to the next mushroom after handling collision with this segment
            }
        }
    }
}
void BCcollision(float bullet[], float centipede[][2], bool centipedeDirection[], const int& numSegments, Sound& collisionSound, int&score) 
{
    int hitSegment = -1; // Variable to store the index of the hit segment
    for (int i = 0; i < numSegments; ++i) 
    {
        if (bullet[x] >= centipede[i][x] && bullet[x] <= centipede[i][x] + boxPixelsX && bullet[y] >= centipede[i][y] && bullet[y] <= centipede[i][y] + boxPixelsY) 
        {
            hitSegment = i; // Store the index of the hit segment
            break; // Exit the loop after finding the hit segment
        }
    }

    if (hitSegment != -1) 
    {
        // Collision occurred with centipede segment hitSegment
        centipede[hitSegment][0] = -10000; // Move the segment outside the visible area
        centipede[hitSegment][1] = -10000;
        bullet[exists] = false; // Destroy the bullet after collision
        collisionSound.play();
        // Change direction for segments behind the hit segment
        for (int j = hitSegment + 1; j < numSegments; ++j) 
        {
            centipedeDirection[j] = !centipedeDirection[j]; // Change direction of the segment
        }
        score += 10;
    }
}
bool CPcollision(float player[], float centipede[][2], const int& numSegments) 
{
    // Check collision between player and any segment of the centipede
    for (int i = 0; i < numSegments; ++i) 
    {
        if (player[0] >= centipede[i][0] && player[0] <= centipede[i][0] + boxPixelsX && player[1] >= centipede[i][1] && player[1] <= centipede[i][1] + boxPixelsY) 
        {
            return true; // Collision detected
        }
    }
    return false; // No collision detected
}




