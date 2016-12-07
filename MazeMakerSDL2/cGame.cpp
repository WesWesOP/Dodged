/*
==================================================================================
cGame.cpp
==================================================================================
*/
#include "cGame.h"

cGame* cGame::pInstance = NULL;
static cTextureMgr* theTextureMgr = cTextureMgr::getInstance();
static cFontMgr* theFontMgr = cFontMgr::getInstance();
static cSoundMgr* theSoundMgr = cSoundMgr::getInstance();
static cButtonMgr* theButtonMgr = cButtonMgr::getInstance();


/*
=================================================================================
Constructor
=================================================================================
*/
cGame::cGame()
{

}
/*
=================================================================================
Singleton Design Pattern
=================================================================================
*/
cGame* cGame::getInstance()
{
	if (pInstance == NULL)
	{
		pInstance = new cGame();
	}
	return cGame::pInstance;
}


void cGame::initialise(SDL_Window* theSDLWND, SDL_Renderer* theRenderer)
{
	// Get width and height of render context
	SDL_GetRendererOutputSize(theRenderer, &renderWidth, &renderHeight);
	this->m_lastTime = high_resolution_clock::now();

	// Clear the buffer with a black background
	SDL_SetRenderDrawColor(theRenderer, 0, 0, 0, 255);
	SDL_RenderPresent(theRenderer);
	
	theTextureMgr->setRenderer(theRenderer);
	theFontMgr->initFontLib(); //initializes the fint library
	theSoundMgr->initMixer(); //initializes the mixer

	theAreaClicked = { 0, 0 };

	// Store the textures
	textureName = {"asteroid1", "asteroid2", "theRocket", "theBackground" }; //creates a texture list 
	texturesToUse = {"Images/MainMenu/asteroid2.png", "Images/MainMenu/asteroid2.png", "Images/MainMenu/rocketSprite.png", "Images/Bkg/bkg.png" }; //actual textures to be displayed 
	for (int tCount = 0; tCount < textureName.size(); tCount++) //makes sure that the number of textures is increased if it is 0
	{
		theTextureMgr->addTexture(textureName[tCount], texturesToUse[tCount]); //gets the textures to be used
	}

	//Store the textures
	btnNameList = { "exit_btn", "instructions_btn", "load_btn", "menu_btn", "play_btn",}; //creates a name list 
	btnTexturesToUse = { "Images/Buttons/button_exit.png", "Images/Buttons/button_instructions.png", "Images/Buttons/button_load.png", "Images/Buttons/button_menu.png", "Images/Buttons/button_play.png"}; //actual textures to be displayed 
	btnPos = { { 400, 375 }, { 400, 300 }, { 400, 300 }, { 500, 500 }, { 400, 300 }, { 740, 500 }, { 400, 300 } }; //positions of the buttons on the screen
	for (int bCount = 0; bCount < btnNameList.size(); bCount++) //makes sure that the number of textures is increased if it is 0
	{
		theTextureMgr->addTexture(btnNameList[bCount], btnTexturesToUse[bCount]); //gets the textures to be used
	}
	for (int bCount = 0; bCount < btnNameList.size(); bCount++) //gets the required buttons and puts the designated textures on them
	{
		cButton * newBtn = new cButton();
		newBtn->setTexture(theTextureMgr->getTexture(btnNameList[bCount]));
		newBtn->setSpritePos(btnPos[bCount]);
		newBtn->setSpriteDimensions(theTextureMgr->getTexture(btnNameList[bCount])->getTWidth(), theTextureMgr->getTexture(btnNameList[bCount])->getTHeight());
		theButtonMgr->add(btnNameList[bCount], newBtn);
	}
	theGameState = MENU;
	theBtnType = EXIT;

	// Create textures for Game Dialogue (text)
	fontList = { "Dark", "ice" }; //creates a font list
	fontsToUse = { "Fonts/Dark Underground.ttf", "Fonts/Dark Underground.ttf" }; //actual textures to be displayed
	for (int fonts = 0; fonts < fontList.size(); fonts++)
	{
		theFontMgr->addFont(fontList[fonts], fontsToUse[fonts], 48); //selects the font
	}

	// Create text Textures
	gameTextNames = { "TitleTxt", "CreateTxt", "DragDropTxt", "ThanksTxt", "GameOver", "Score"};
	gameTextList = { "Dodged", "Dodge all the rocks", "Use The arrow keys to move", "Thank you", "Game Over", "Score"};
	for (int text = 0; text < gameTextNames.size(); text++)
	{
		theTextureMgr->addTexture(gameTextNames[text], theFontMgr->getFont("Dark")->createTextTexture(theRenderer, gameTextList[text], SOLID, { 64, 37, 37, 255 }, { 0, 0, 0, 0 })); //selects the font and the color of the font
	}

	// Load game sounds
	soundList = { "theme", "click" }; //creates a sound list
	soundTypes = { MUSIC, SFX }; //sets the type of the sounds
	soundsToUse = { "Audio/Theme/ThemeSong.wav", "Audio/SFX/ButtonClick.wav"}; //shows the exact sound .wav fiels that will be used
	for (int sounds = 0; sounds < soundList.size(); sounds++)
	{
		theSoundMgr->add(soundList[sounds], soundsToUse[sounds], soundTypes[sounds]);
	}
	theSoundMgr->getSnd("theme")->play(-1);

	spriteBkgd.setSpritePos({ 0, 0 }); //sets the positon of the background position
	spriteBkgd.setTexture(theTextureMgr->getTexture("theBackground")); //gets the bck image
	spriteBkgd.setSpriteDimensions(theTextureMgr->getTexture("theBackground")->getTWidth(), theTextureMgr->getTexture("theBackground")->getTHeight()); //sets the dimensions

	theRocket.setSpritePos({ 500, 620 }); // sets the characters position
	theRocket.setTexture(theTextureMgr->getTexture("theRocket")); //gets the texture to be used
	theRocket.setSpriteDimensions(theTextureMgr->getTexture("theRocket")->getTWidth(), theTextureMgr->getTexture("theRocket")->getTHeight()); //sets the the dimensions
	theRocket.setRocketVelocity({ 0, 0 }); //sets the velocity
	
	// Create vector array of textures
	for (int astro = 0; astro < 5; astro++) 
	{
		theAsteroids.push_back(new cAsteroid);
		theAsteroids[astro]->setSpritePos({ 150 * (rand() % 5 + 1)*(rand() % 5 + 1) - 100 });
		theAsteroids[astro]->setSpriteTranslation({ (rand() % 8 + 1), (rand() % 8 + 1) });
		int randAsteroid = rand() % 4;
		theAsteroids[astro]->setTexture(theTextureMgr->getTexture(textureName[randAsteroid]));
		theAsteroids[astro]->setSpriteDimensions(theTextureMgr->getTexture(textureName[randAsteroid])->getTWidth(), theTextureMgr->getTexture(textureName[randAsteroid])->getTHeight());
		theAsteroids[astro]->setAsteroidVelocity({ 3.0f, 3.0f });
		theAsteroids[astro]->setActive(true);
	}
	
}

void cGame::run(SDL_Window* theSDLWND, SDL_Renderer* theRenderer)
{
	loop = true;

	while (loop)
	{
		//We get the time that passed since the last frame
		double elapsedTime = this->getElapsedSeconds();

		loop = this->getInput(loop);
		this->update(elapsedTime);
		this->render(theSDLWND, theRenderer);
	}
}

void cGame::render(SDL_Window* theSDLWND, SDL_Renderer* theRenderer)
{
	SDL_RenderClear(theRenderer); //clears the renderer
	switch (theGameState) //swtich statement to go through the different game states
	{
	case MENU: //case while at the menu screen
	{
		spriteBkgd.render(theRenderer, NULL, NULL, spriteBkgd.getSpriteScale()); 
		// Render the Title
		tempTextTexture = theTextureMgr->getTexture("TitleTxt"); //gets the texture
		pos = { 10, 10, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h }; //sets its position
		scale = { 1, 1 }; //sets the scale
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale); //renders the texture
		tempTextTexture = theTextureMgr->getTexture("CreateTxt"); //gets the texture
		pos = { 300, 10, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h }; //sets its position
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale); //renders the texture
		tempTextTexture = theTextureMgr->getTexture("DragDropTxt"); //gets the texture
		pos = { 300, 75, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h }; //sets its position
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale); //renders the texture
		// Render Button //sets the place and renders the play and exit buttons located in the mainmenu
		theButtonMgr->getBtn("play_btn")->render(theRenderer, &theButtonMgr->getBtn("play_btn")->getSpriteDimensions(), &theButtonMgr->getBtn("play_btn")->getSpritePos(), theButtonMgr->getBtn("play_btn")->getSpriteScale());
		theButtonMgr->getBtn("exit_btn")->setSpritePos({ 400, 375 });
		theButtonMgr->getBtn("exit_btn")->render(theRenderer, &theButtonMgr->getBtn("exit_btn")->getSpriteDimensions(), &theButtonMgr->getBtn("exit_btn")->getSpritePos(), theButtonMgr->getBtn("exit_btn")->getSpriteScale());
	}
	break;
	case PLAYING: //case while at the playing screen
	{ //sets the texture and location of the exit button while the game state is Playing and then renders it on the screenw
		spriteBkgd.render(theRenderer, NULL, NULL, spriteBkgd.getSpriteScale());
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);
		pos = { 10, 10, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		theButtonMgr->getBtn("exit_btn")->setSpritePos({ 770, 575 });
		theButtonMgr->getBtn("exit_btn")->render(theRenderer, &theButtonMgr->getBtn("exit_btn")->getSpriteDimensions(), &theButtonMgr->getBtn("exit_btn")->getSpritePos(), theButtonMgr->getBtn("exit_btn")->getSpriteScale());

		// Render each asteroid in the vector array
		for (int draw = 0; draw < theAsteroids.size(); draw++) //increases the number of asteroids
		{
			theAsteroids[draw]->render(theRenderer, &theAsteroids[draw]->getSpriteDimensions(), &theAsteroids[draw]->getSpritePos(), theAsteroids[draw]->getSpriteRotAngle(), &theAsteroids[draw]->getSpriteCentre(), theAsteroids[draw]->getSpriteScale()); //renders the asteroids
		}


		// Render the Score
		theTextureMgr->addTexture("Score", theFontMgr->getFont("Dark")->createTextTexture(theRenderer, sScore.c_str(), SOLID, { 64, 37, 37, 255 }, { 0, 0, 0, 0 })); //sets the score texts font and color
		tempTextTexture = theTextureMgr->getTexture("Score"); //sets the score texts font and color
		pos = { 780, 10, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h }; //sets the position
		scale = { 1, 1 }; //sets the scale
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale); //renders the score

		// Render the rocket
		theRocket.render(theRenderer, &theRocket.getSpriteDimensions(), &theRocket.getSpritePos(), theRocket.getSpriteRotAngle(), &theRocket.getSpriteCentre(), theRocket.getSpriteScale());
		SDL_RenderPresent(theRenderer);

		// Update the score
		theScore += 1; //adds 1 to the score every frame the game is running
		sScore = gameTextList[5] + to_string(theScore); //picks the score text form the created textlist and adds the theScore string to it
		theTextureMgr->deleteTexture("Score"); //deletes the Score texture in order to update it every frame

	}
	break;

	case END: //case when at the end screen
	{
		spriteBkgd.render(theRenderer, NULL, NULL, spriteBkgd.getSpriteScale());
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);
		tempTextTexture = theTextureMgr->getTexture("GameOver"); //gets the game over texture
		pos = { 370, 75, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h }; //sets the position
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale); //renders the game over texture
		theButtonMgr->getBtn("menu_btn")->setSpritePos({ 400, 450 }); //sets the positoon of the menu button
		theButtonMgr->getBtn("menu_btn")->render(theRenderer, &theButtonMgr->getBtn("menu_btn")->getSpriteDimensions(), &theButtonMgr->getBtn("menu_btn")->getSpritePos(), theButtonMgr->getBtn("menu_btn")->getSpriteScale()); //renders the menu button
		theButtonMgr->getBtn("exit_btn")->setSpritePos({ 400, 525 });
		theButtonMgr->getBtn("exit_btn")->render(theRenderer, &theButtonMgr->getBtn("exit_btn")->getSpriteDimensions(), &theButtonMgr->getBtn("exit_btn")->getSpritePos(), theButtonMgr->getBtn("exit_btn")->getSpriteScale());
		theTextureMgr->addTexture("Score", theFontMgr->getFont("Dark")->createTextTexture(theRenderer, sScore.c_str(), SOLID, { 64, 37, 37, 255 }, { 0, 0, 0, 0 })); //sets the score texts font and color
		tempTextTexture = theTextureMgr->getTexture("Score"); //sets the score texts font and color
		pos = { 400, 200, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h }; //sets the position
		scale = { 1, 1 }; //sets the scale
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale); //renders the score
	}
	break;
	case QUIT: //case for when the 

	{
		loop = false;
	}
	break;
	default:
		break;
	}
	SDL_RenderPresent(theRenderer); 
}

void cGame::render(SDL_Window* theSDLWND, SDL_Renderer* theRenderer, double rotAngle, SDL_Point* spriteCentre)
{

	SDL_RenderPresent(theRenderer);
}

void cGame::update()
{

}

void cGame::update(double deltaTime)
{
	// Check Button clicked and change state
	switch (theGameState)
	{
	case MENU:
	{
				 theGameState = theButtonMgr->getBtn("exit_btn")->update(theGameState, QUIT, theAreaClicked);
				 theGameState = theButtonMgr->getBtn("play_btn")->update(theGameState, PLAYING, theAreaClicked);
	}
		break;
	case PLAYING:
	{
					theGameState = theButtonMgr->getBtn("exit_btn")->update(theGameState, END, theAreaClicked);

					for (vector<cAsteroid*>::iterator asteroidIterator = theAsteroids.begin(); asteroidIterator != theAsteroids.end(); ++asteroidIterator)
					{
						if ((*asteroidIterator)->collidedWith(&(*asteroidIterator)->getBoundingRect(), &theRocket.getBoundingRect()))
						{
							theGameState = END;
						}
					}

					// Update the visibility and position of each asteriod
					vector<cAsteroid*>::iterator asteroidIterator = theAsteroids.begin();
					while (asteroidIterator != theAsteroids.end())
					{
						if ((*asteroidIterator)->isActive() == false)
						{
							asteroidIterator = theAsteroids.erase(asteroidIterator);
						}
						else
						{
							(*asteroidIterator)->update(deltaTime);
							++asteroidIterator;
						}
					}

					// Update the Rockets position
					theRocket.update(deltaTime);
	}

					break;
	
	case END:
	{
				theGameState = theButtonMgr->getBtn("exit_btn")->update(theGameState, QUIT, theAreaClicked);
				theGameState = theButtonMgr->getBtn("menu_btn")->update(theGameState, MENU, theAreaClicked);
	}
		break;
	case QUIT:
	{
	}
		break;
	default:
		break;


	}
	}


bool cGame::getInput(bool theLoop)
{
	SDL_Event event;

	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_QUIT)
		{
			theLoop = false;
		}
		switch (event.type)
		{
			case SDL_MOUSEBUTTONDOWN:
				switch (event.button.button)
				{
				case SDL_BUTTON_LEFT:
				{
					theAreaClicked = { event.motion.x, event.motion.y };
				}
				break;
				case SDL_BUTTON_RIGHT:
					break;
				default:
					break;
				}
				break;
			case SDL_MOUSEBUTTONUP:
				switch (event.button.button)
				{
				case SDL_BUTTON_LEFT:
				{	
				}
				break;
				case SDL_BUTTON_RIGHT:
					break;
				default:
					break;
				}
				break;
			case SDL_MOUSEMOTION:
			{
				
			}
			break;
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym)
				{
				case SDLK_ESCAPE:
					theLoop = false;
					break;
				case SDLK_DOWN:
				{
				}
				break;

				case SDLK_UP:
				{
				}
				break;
				case SDLK_RIGHT:
				{
					 theRocket.setSpritePos({ theRocket.getSpritePos().x + 15, theRocket.getSpritePos().y }); //changes the position of the sprite on the x axis so it moves to the right
				}
				break;

				case SDLK_LEFT:
				{
				    theRocket.setSpritePos({ theRocket.getSpritePos().x - 15, theRocket.getSpritePos().y }); //changes the position of the sprite on the x axis so it moves to the left
				}
				break;
				case SDLK_SPACE:
				{
				}
				break;
				default:
					break;
				}

			default:
				break;
		}

	}
	return theLoop;
}

double cGame::getElapsedSeconds()
{
	this->m_CurrentTime = high_resolution_clock::now();
	this->deltaTime = (this->m_CurrentTime - this->m_lastTime);
	this->m_lastTime = this->m_CurrentTime;
	return deltaTime.count();
}

void cGame::cleanUp(SDL_Window* theSDLWND)
{
	// Delete our OpengL context
	SDL_GL_DeleteContext(theSDLWND);

	// Destroy the window
	SDL_DestroyWindow(theSDLWND);

	// Quit IMG system
	IMG_Quit();

	// Shutdown SDL 2
	SDL_Quit();
}

