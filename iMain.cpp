#include "iGraphics.h"
#include <assert.h>
#include <math.h>
#include <time.h>
#include <Windows.h>

void drawhome(void);
void drawbg(void);
void drawgame(void);
void drawgameover(void);
void drawnamescreen(void);
void drawleaderboard(void);
void drawinstructions(void);
void drawabout(void);
void showplayer(void);
void shoot(void);
void showbullets(void);
void showasteroids(void);
void explosion(void);
void initasteroids(void);
void hitcheck(void);
void collision(void);
void leaderboardupdate(char name[], int score);
void reset(void);
void scorebar(void);

int t1, t2, t3; // t1 for asteroid initializer, t2 for explosion conntrol, t3 for levels
int level = 1;

const int screenwidth = 648, screenheight = 864;

int home = 1, start = 0, PlayerScore = 0, gameover = 0, namescreen = 0, leaderboardscreen = 0, about = 0, instruction = 0;

double playerx = 285, playery = 5;
double x_ellipse[25], y_ellipse[25]; // spaceship is divided into an ellipse and a triangle this array store 25  ellipse points for each position of the spaceship
char name[50];
int nameind = -1;

bool music = true;

typedef struct bullets
{
	double X;
	double Y;
	bool active = false;
};

typedef struct asteroids
{
	double X, Y;
	double velocity;
	bool alive = false;
};

asteroids asteroid[12];
int maxasteroids = 5;
double minasteroidvelocity = 2.5;

char explosionbmpname[4][30] = {"Explosion\\tile000.bmp", "Explosion\\tile001.bmp", "Explosion\\tile002.bmp", "Explosion\\tile003.bmp"};
int explosionind = 0;
int exploding = 0;
double explosionX, explosionY;

bullets bullet[60];
const int maxbullets = 60;
const int bulletvelocity = 10;
int bulletind = 0;

void iDraw()
{
	// place your drawing codes here
	iClear();
	if (home == 1)
	{
		drawhome();
	}
	if (start == 1)
	{
		drawgame();
	}
	if (gameover == 1)
	{
		drawgameover();
	}
	if (namescreen == 1)
	{
		drawnamescreen();
	}
	if (leaderboardscreen == 1)
	{
		drawleaderboard();
	}
	if (about == 1)
	{
		drawabout();
	}
	if (instruction == 1)
	{
		drawinstructions();
	}
}

/*
	function iMouseMove() is called when the user presses and drags the mouse.
	(mx, my) is the position where the mouse pointer is.
	*/
void iMouseMove(int mx, int my)
{
}

/*
	function iMouse() is called when the user presses/releases the mouse.
	(mx, my) is the position where the mouse pointer is.
	*/
void iMouse(int button, int state, int mx, int my)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		if (home == 1 && namescreen == 0 && mx > 170 && mx < 478 && my > 476 && my < 582) // show enter name screen
		{
			home = 0;
			namescreen = 1;
		}
		if (namescreen == 1 && home == 0 && mx > 0 && mx < 197 && my > 0 && my < 66) // go back to home screen
		{
			nameind = -1;
			name[0] = '\0';
			namescreen = 0;
			home = 1;
		}

		if (home == 1 && instruction == 0 && mx > 154 && mx < 494 && my > 227 && my < 334) // show instruction screen
		{
			home = 0;
			instruction = 1;
		}
		if (instruction == 1 && home == 0 && mx > 0 && mx < 197 && my > 0 && my < 66) // go back to home screen
		{
			instruction = 0;
			home = 1;
		}

		if (home == 1 && about == 0 && mx > 241 && mx < 406 && my > 187 && my < 245) // show about screen
		{
			home = 0;
			about = 1;
		}
		if (about == 1 && home == 0 && mx > 0 && mx < 197 && my > 0 && my < 66) // go back to about screen
		{
			about = 0;
			home = 1;
		}

		if (home == 1 && leaderboardscreen == 0 && mx > 160 && mx < 485 && my > 94 && my < 154) // show leaderboard screen
		{
			home = 0;
			leaderboardscreen = 1;
		}
		if (leaderboardscreen == 1 && home == 0 && mx > 0 && mx < 197 && my > 0 && my < 66) // go back to home screen
		{
			leaderboardscreen = 0;
			home = 1;
		}
		if (home == 1 && mx > 227 && mx < 286 && my > 21 && my < 76 && music == false) // music on button
		{
			music = true;
			PlaySound("Sounds\\BG.wav", NULL, SND_LOOP | SND_ASYNC);
		}
		if (home == 1 && mx > 362 && mx < 418 && my > 20 && my < 72 && music == true) // music off button
		{
			music = false;
			PlaySound(0, 0, 0);
		}

		if (home == 1 && mx > 214 && mx < 432 && my > 368 && my < 444) // exit button
		{
			home = 0;
			exit(0);
		}
		if (gameover == 1 && home == 0 && mx > 0 && mx < screenwidth && my > 0 && my < screenheight) // go to home from game over
		{
			PlayerScore = 0;
			gameover = 0;
			home = 1;
		}

		// printf("x = %d, y= %d\n", mx, my);
	}
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{
	}
}

/*
	function iKeyboard() is called whenever the user hits a key in keyboard.
	key- holds the ASCII value of the key pressed.
	*/
void iKeyboard(unsigned char key)
{
	if (start == 1 && key == ' ') // bullet shoot kore space button tip dile
	{
		shoot();
	}
	if (namescreen == 1)
	{
		if (nameind != -1 && key == '\b') // kono nam thakle backspace diye ekta character erase kore, kono nam na thakle kaaj korena
		{
			name[nameind] = '\0';
			nameind--;
		}
		else if (nameind != -1 && key == '\r') // enter tip dile start hoy, kono nam na dile kaj korbe na
		{
			namescreen = 0;
			start = 1;		  // game start
			iResumeTimer(t1); // animation start
			iResumeTimer(t3); // leveler start
		}
		else if (key != '\r' && key != ' ' && nameind < 19) // 20 ta character nibe highest, space dile void korbe
		{
			nameind++;
			name[nameind] = key;
			name[nameind + 1] = '\0';
		}
	}
}

/*
	function iSpecialKeyboard() is called whenver user hits special keys like-
	function keys, home, end, pg up, pg down, arraows etc. you have to use
	appropriate constants to detect them. A list is:
	GLUT_KEY_F1, GLUT_KEY_F2, GLUT_KEY_F3, GLUT_KEY_F4, GLUT_KEY_F5, GLUT_KEY_F6,
	GLUT_KEY_F7, GLUT_KEY_F8, GLUT_KEY_F9, GLUT_KEY_F10, GLUT_KEY_F11, GLUT_KEY_F12,
	GLUT_KEY_LEFT, GLUT_KEY_UP, GLUT_KEY_RIGHT, GLUT_KEY_DOWN, GLUT_KEY_PAGE UP,
	GLUT_KEY_PAGE DOWN, GLUT_KEY_HOME, GLUT_KEY_END, GLUT_KEY_INSERT
	*/
void iSpecialKeyboard(unsigned char key)
{

	if (key == GLUT_KEY_END) // emergency exit
	{
		exit(0);
	}
	if (start == 1 && key == GLUT_KEY_RIGHT && playerx < 553)
	{
		playerx += 20;				 // player right move kore until screen er end e pouchay
		for (int i = 0; i < 25; i++) // prottekbar move korlei ellipse points bair kora hoy
		{
			x_ellipse[i] = 40 * cos(i * 360 / 25) + (playerx + 40);
			y_ellipse[i] = 52.5 * sin(i * 360 / 25) + (playery + 74.4);
		}
	}
	if (start == 1 && key == GLUT_KEY_LEFT && playerx > 13)
	{
		playerx -= 20; // same as above for left movement
		for (int i = 0; i < 25; i++)
		{
			x_ellipse[i] = 40 * cos(i * 360 / 25) + (playerx + 40);
			y_ellipse[i] = 52.5 * sin(i * 360 / 25) + (playery + 74.4);
		}
	}
}
// game gfx codes

void drawgame(void)
{
	drawbg();
	showbullets();
	showasteroids();
	explosion();
	scorebar();
	hitcheck();
	collision();
	showplayer();
}

void drawhome(void)
{
	iShowBMP(0, 0, "Screens\\home.bmp");
}

void drawbg(void)
{
	iShowBMP(0, 0, "Screens\\Background.bmp");
}

void drawgameover(void)
{
	iShowBMP2(0, 0, "Screens\\GameOver.bmp", -1);
	iSetColor(0, 0, 0);
	iText(243, 410, "Score: ", GLUT_BITMAP_TIMES_ROMAN_24);
	char score[10]; // eta string srteam, to display the score in screen
	sprintf(score, "%d", PlayerScore);
	iText(360, 410, score, GLUT_BITMAP_TIMES_ROMAN_24);
}

void drawnamescreen(void)
{
	iShowBMP2(0, 0, "Screens\\Namescreen.bmp", -1);
	iSetColor(0, 0, 0);
	iText(215, 453, name, GLUT_BITMAP_TIMES_ROMAN_24);
}
void drawinstructions(void)
{
	iShowBMP2(0, 0, "Screens\\Instructions.bmp", -1);
}
void drawabout(void)
{
	iShowBMP2(0, 0, "Screens\\About.bmp", -1);
}
void drawleaderboard(void)
{
	iShowBMP2(0, 0, "Screens\\Leaderboard.bmp", -1);
	char names[200][50];					 // shob nam load korar jayga
	int scores[200];						 // shob score load korar jayga
	FILE *f = fopen("leaderboard.txt", "r"); // file open in read mode
	assert(f);
	int i;
	for (i = 0; i < 200 && fscanf(f, "%s %d", names[i], &scores[i]) != EOF; i++) // shob nam ar score load kore dey
	{
		continue;
	}
	fclose(f);						// load er kaj shesh close the file
	for (int m = 0; m < i - 1; m++) // bubblesort
	{
		for (int n = 0; n < i - m - 1; n++)
		{
			if (scores[n] < scores[n + 1])
			{
				int tempScore = scores[n];
				scores[n] = scores[n + 1];
				scores[n + 1] = tempScore;
				char tempName[50];
				strcpy(tempName, names[n]);
				strcpy(names[n], names[n + 1]);
				strcpy(names[n + 1], tempName);
			}
		}
	}
	iSetColor(255, 255, 255);
	for (int j = 0; j < i && j < 10; j++) // namgula print kore
	{
		iText(172, 633 - j * 58, names[j], GLUT_BITMAP_TIMES_ROMAN_24);
	}
	for (int j = 0; j < i && j < 10; j++) // scoregula print kore
	{

		char scorestream[10];
		sprintf(scorestream, "%d", scores[j]);
		iText(500, 633 - j * 58, scorestream, GLUT_BITMAP_TIMES_ROMAN_24);
	}
}

void showplayer(void)
{
	iShowBMP2(playerx, playery, "Sprites\\craft.bmp", 0xFFFFFF);
}

void initasteroids(void) // this function is called by t1 timer after every 100ms
{

	for (int i = 0; i < maxasteroids; i++)
	{
		if (!asteroid[i].alive) // jodi kono asteroid deactive thake oigula active kore
		{
			asteroid[i].alive = true;
			asteroid[i].Y = rand() % 100 + screenheight;			 // random y position above screen
			asteroid[i].X = rand() % 518 + 30;						 // random x position above screen
			asteroid[i].velocity = rand() % 2 + minasteroidvelocity; // random velocity, minimumm velocity change hobe kisukhon porpor
		}
	}
}

void showasteroids(void)
{
	for (int i = 0; i < maxasteroids; i++)
	{
		if (asteroid[i].alive) // asteroid active thakle dekhabe
		{
			if (asteroid[i].Y < 25) //asteroid screen er niche gele deactivate korbe
			{
				exploding = 1;                                    //explosion hobe
				explosionind = 0;                                  //explosion er starting frame
				explosionX = asteroid[i].X;                        //ei X position e explosion hobe
				explosionY = asteroid[i].Y;
				asteroid[i].alive=false;
			}
			iShowBMP2(asteroid[i].X, asteroid[i].Y, "Sprites\\asteroid.bmp", 0xFFFFFF);
			asteroid[i].Y -= asteroid[i].velocity;    //asteroid er y position per call e change hobe
		}
	}
}

void shoot(void)
{
	if (bulletind == maxbullets - 1)         //60 ta bullet ber hoile rewind
	{
		bulletind = 0;
	}
	bullet[bulletind].active = true;           //space tiple ekta ekta kore bullet ke active korbe
	bullet[bulletind].X = playerx + 33.5;
	bullet[bulletind].Y = 173;
	bulletind++;
}

void showbullets(void)
{
	for (int i = 0; i < maxbullets; i++)
	{
		if (bullet[i].active)                     //bullet active thakle dekhabe
		{
			if (bullet[i].Y > screenheight - 50)         //bullet screen er baire gele deactivate korbe
			{
				bullet[i].active = false;
			}

			iShowBMP2(bullet[i].X, bullet[i].Y, "Sprites\\bullet.bmp", 0);
			bullet[i].Y += bulletvelocity;
		}
	}
}

void hitcheck(void)
{
	for (int i = 0; i < maxasteroids; i++)               //prottekta active bullet ar active asteroid er modhhe collision check
	{
		for (int j = 0; j < maxbullets; j++)
		{
			double ctoc = sqrt(((asteroid[i].X + 35 - bullet[j].X - 6) * (asteroid[i].X + 35 - bullet[j].X - 6)) + ((asteroid[i].Y + 35 - bullet[j].Y - 5.5) * (asteroid[i].Y + 35 - bullet[j].Y - 5.5)));
			if (ctoc <= 31 && bullet[j].active && asteroid[i].alive)               //jodi center to center distance r1+r2 er theke kom ba shoman hoy tar mane collision hoise
			{
				exploding = 1;                                    //explosion hobe
				explosionind = 0;                                  //explosion er starting frame
				explosionX = asteroid[i].X;                        //ei X position e explosion hobe
				explosionY = asteroid[i].Y;                        //ei Y position e explosion hobe
				bullet[j].active = false;                           //bullet deactivate hobe
				asteroid[i].alive = false;                         //asteroid dead hobe
				PlayerScore++;                                     //score barbe
			}
		}
	}
}
void explosion(void)                                       //explosion idraw te call hoite thakbe
{
	if (exploding)                                         //jodi collision hoy tahole collision er X,Y point e framing animation hobe
	{
		iResumeTimer(t2);                                  //t2 timer collision frame change kore
		iShowBMP2(explosionX, explosionY, explosionbmpname[explosionind], 0xFFFFFF);
	}
}
void explosionframer(void)                               //this function is being called by timer t2 every 50ms after there is an explosion
{
	if (explosionind == 3)                               //explosion er 4 ta frame ase, 4 no. e pouchale t2 timer pause and explosion boolean becomes false
	{
		exploding = 0;
		iPauseTimer(t2);
	}
	else
	{
		explosionind++;                               //onno frame er khetre 50ms porpor fram change i.e. next frame e jabe
	}
}

void collision(void)
{
	double x[7] = {playerx + 30, playerx + 30, playerx + 33.34, playerx + 39, playerx + 44, playerx + 48, playerx + 48}, y[7] = {playery + 117, playery + 135, playery + 152.5, playery + 178, playery + 152.5, playery + 135, playery + 117};

	for (int i = 0; i < maxasteroids; i++)
	{
		if (asteroid[i].alive)
		{
			double asteroid_centre_x = asteroid[i].X + 35, asteroid_centre_y = asteroid[i].Y + 35;
			for (int j = 0; j < 25; j++)
			{
				if (sqrt((asteroid_centre_x - x_ellipse[j]) * (asteroid_centre_x - x_ellipse[j]) + (asteroid_centre_y - y_ellipse[j]) * (asteroid_centre_y - y_ellipse[j])) <= 25)
				{
					asteroid[i].alive = false;
					start = 0;
					gameover = 1;
					leaderboardupdate(name, PlayerScore);
					reset();
					break;
				}
			}

			for (int j = 0; j < 7; j++)
			{
				if (sqrt((asteroid_centre_x - x[j]) * (asteroid_centre_x - x[j]) + (asteroid_centre_y - y[j]) * (asteroid_centre_y - y[j])) <= 25)
				{
					asteroid[i].alive = false;
					start = 0;
					gameover = 1;

					leaderboardupdate(name, PlayerScore);
					reset();
					break;
				}
			}
		}
	}
}

void scorebar(void)
{
	iSetColor(0, 0, 0);
	iFilledRectangle(0, screenheight - 30, screenwidth, 30);
	iSetColor(255, 255, 255);
	iText(290, 840, "Score: ", GLUT_BITMAP_HELVETICA_18);
	char scorestream[10];
	sprintf(scorestream, "%d", PlayerScore);
	iText(360, 840, scorestream, GLUT_BITMAP_HELVETICA_18);
}

void leaderboardupdate(char name[], int score)
{
	FILE *f = fopen("leaderboard.txt", "a");
	assert(f);
	fprintf(f, "%s %d\n", name, score);
	fclose(f);
}

void leveler(void)
{
	if (PlayerScore >= 250 && level == 6)
	{
		level = 7;
		printf("level 7");
		minasteroidvelocity = 8;
		maxasteroids = 10;
	}
	if (PlayerScore >= 200 && PlayerScore < 250 && level == 5)
	{
		level = 6;
		printf("level 6");
		minasteroidvelocity = 7.5;
		maxasteroids = 10;
	}
	if (PlayerScore >= 150 && PlayerScore < 200 && level == 4)
	{
		level = 5;
		printf("level 5");
		minasteroidvelocity = 6.5;
		maxasteroids = 9;
	}
	if (PlayerScore >= 100 && PlayerScore < 150 && level == 3)
	{
		level = 4;
		printf("level 4");
		minasteroidvelocity = 5.5;
		maxasteroids = 8;
	}
	if (PlayerScore >= 50 && PlayerScore < 100 && level == 2)
	{
		level = 3;
		printf("level 3");
		minasteroidvelocity = 4.5;
		maxasteroids = 7;
	}
	if (PlayerScore >= 25 && PlayerScore < 50 && level == 1)
	{
		level = 2;
		printf("level 2");
		minasteroidvelocity = 3.5;
		maxasteroids = 6;
	}
}
void reset(void)
{
	iPauseTimer(t1);
	iPauseTimer(t2);
	iPauseTimer(t3);
	level = 1;
	exploding = 0;
	playerx = 285;
	nameind = -1;
	name[nameind + 1] = '\0';
	minasteroidvelocity = 2.5;
	maxasteroids = 5;
	for (int i = 0; i < maxbullets; i++)
	{
		bullet[i].active = false;
	}
	for (int i = 0; i < maxasteroids; i++)
	{
		asteroid[i].alive = false;
	}
	for (int i = 0; i < 25; i++)
	{
		x_ellipse[i] = -1;
		y_ellipse[i] = -1;
	}
}

int main()
{
	t1 = iSetTimer(100, initasteroids);
	iPauseTimer(t1);
	t2 = iSetTimer(50, explosionframer);
	iPauseTimer(t2);
	t3 = iSetTimer(1000, leveler);
	iPauseTimer(t3);
	srand(time(NULL));
	PlaySound("Sounds\\BG.wav", NULL, SND_LOOP | SND_ASYNC);
	iInitialize(648, 864, "Asteroid Shooter");
	return 0;
}