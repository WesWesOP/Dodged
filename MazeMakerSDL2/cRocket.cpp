/*
=================
cRocket.cpp
- Header file for class definition - IMPLEMENTATION
=================
*/
#include "cRocket.h"

/*
=================================================================
Defualt Constructor
=================================================================
*/
cRocket::cRocket() : cSprite()
{
	this->rocketVelocity = { 0.0f, 0.0f };
}
/*
=================================================================
Update the sprite position
=================================================================
*/

void cRocket::update(double deltaTime)
{

	FPoint direction = { 0.0f, 0.0f };
	direction.X = (sin(this->getSpriteRotAngle()*PI / 180));
	direction.Y = -(cos(this->getSpriteRotAngle()*PI / 180));

	direction.X *= this->getSpriteTranslation().x;
	direction.Y *= this->getSpriteTranslation().y;

	this->rocketVelocity.x = this->rocketVelocity.x + direction.X;
	this->rocketVelocity.y = this->rocketVelocity.y + direction.Y;

	SDL_Rect currentSpritePos = this->getSpritePos();
	currentSpritePos.x += this->rocketVelocity.x * deltaTime;
	currentSpritePos.y += this->rocketVelocity.y * deltaTime;

	this->rocketVelocity.x *= 0.95;
	this->rocketVelocity.y *= 0.95;

	this->setSpritePos({ currentSpritePos.x, currentSpritePos.y });
	this->setBoundingRect(this->getSpritePos());

	if (currentSpritePos.x > (940)) //if statement to check for the current position of the sprite
	{
		setSpritePos({ 939, currentSpritePos.y }); //sets a certain number on the x axis to limit the movement of the character sprite
	}
	if (currentSpritePos.x < 0) //if statement to check for the current position of the sprite
	{
		setSpritePos({ 0, currentSpritePos.y }); //sets a certain number on the x axis to limit the movement of the character sprite
	}
}
/*
=================================================================
Sets the velocity for the rocket
=================================================================
*/
void cRocket::setRocketVelocity(SDL_Point rocketVel)
{
	rocketVelocity = rocketVel;
}
/*
=================================================================
Gets the rocket velocity
=================================================================
*/
SDL_Point cRocket::getRocketVelocity()
{
	return rocketVelocity;
}