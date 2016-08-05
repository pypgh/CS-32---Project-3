#include "Actor.h"
#include "StudentWorld.h"
#include <queue>
// Students:  Add code to this file (if you wish), Actor.h, StudentWorld.h, and StudentWorld.cpp



StudentWorld* Actor::getWorld() const
{
	return m_world;
}

bool Actor::isAlive() const
{
	return m_alive;										//Return the status: true = alive, false = dead.
}

void Actor::setDead()									//Set to dead by making the alive bool false.
{
	m_alive = false;
	setVisible(false);
}



bool Actor::sprayBlocksZumi(int nextX, int nextY) const
{
	return getWorld()->zumiBlockedBySprayer(nextX, nextY);
}

void Actor::changeWalkThrough()							//Reverse the bool for walking through bricks.
{
	if (canBeWalkedThrough == false)
		canBeWalkedThrough = true;
	else if (canBeWalkedThrough == true)		
		canBeWalkedThrough = false;
}

bool Actor::walkThrough() const
{
	return canBeWalkedThrough;									//Returns if an actor can be walked through by the player. 
}


int Player::OrigNumSprayers()
{
	return orig_sprayers_num;
}

void Player::doSomething()
{	
	//1.
	if (isAlive() == false)													//If the player is dead, return from the function.
		return;
	//2.
	if (getWorld()->colocatedDead(getX(), getY()) == true)		//If the player is colocated with a deadly actor, it dies.
	{
		setDead();
		getWorld()->playSound(SOUND_PLAYER_DIE);
		return;
	}
	//3. Walk through bricks state
	//4. Drop a large number of simultaneous bug sprayers
	

	//5. Check to see if user pressed a key. 
		//Still need to implement for the walk through walls goodie.
		//As well as a bug sprayer use method.
	int ch;																	//Initialize a char for the switch expression
	if (getWorld()->getKey(ch))												//Use the getKey method to get keys in real time
	{
		switch (ch)
		{
			case KEY_PRESS_LEFT:											//Left? move left
				if (!getWorld()->willBeAtSamePlace(getX() - 1, getY()))					//Move only if the next spot is not a brick (use WalkThrough for brick)
				moveTo(getX() - 1, getY());
				break;
			case KEY_PRESS_RIGHT:
				if (!getWorld()->willBeAtSamePlace(getX() + 1, getY()))							//Right? move right
				moveTo(getX() + 1, getY());
				break;
			case KEY_PRESS_DOWN:											//Down? move down
				if (!getWorld()->willBeAtSamePlace(getX(), getY() - 1))
				moveTo(getX(), getY() - 1);
				break;
			case KEY_PRESS_UP:
				if (!getWorld()->willBeAtSamePlace(getX(), getY() + 1))							//Up? move up
				moveTo(getX(), getY() + 1);
				break;	
			case KEY_PRESS_SPACE:											//Drop a thing if any
				if (getWorld()->numOfSprayersOut() < num_sprayers)
					getWorld()->addSprayerIfPossible(getX(), getY());
				break;
		}

	}



}

void Player::increaseSimultaneousSprayers(unsigned int max, unsigned int lifetime)
{
	//Will pass in new max if we want to change, pass in old max if we want to change back.
	if (getWorld()->getMaxSprayers() == max)				//If we're increasing to max
	num_sprayers = max;
	if (orig_sprayers_num == max)							//Changing back.
		num_sprayers = orig_sprayers_num;
}


void DestrBrick::doSomething()													//Set the brick to dead
{
	if (getWorld()->WillDie(getX(), getY()) == true)
	{
		setDead();
	}

}


void Zumi::decreaseTicksOrReset()
{
	if (z_ticksPerMove == 0)
		z_ticksPerMove = constTicks;				//Resets
	else z_ticksPerMove--;
}

int Zumi::returnTicksPerMove() const
{
	return z_ticksPerMove;
}


void Zumi::deadEnd(int decider)
{
	switch(decider)
	{
		case 0:
			if (getWorld()->ZumiBLocked(getX() + 1, getY()) || sprayBlocksZumi(getX() + 1, getY()))
			{
				continueThatDirection = false;
			}
			break;
		case 1:
			if (getWorld()->ZumiBLocked(getX() - 1, getY())|| sprayBlocksZumi(getX() - 1, getY()))
			{
				continueThatDirection = false;
			}
			break;
		case 2:
			if (getWorld()->ZumiBLocked(getX(), getY() + 1)|| sprayBlocksZumi(getX(), getY() + 1))
			{
				continueThatDirection = false;
			}
			break;
		case 3:
			if (getWorld()->ZumiBLocked(getX(), getY() - 1)|| sprayBlocksZumi(getX(), getY() - 1))
			{
				continueThatDirection = false;
			}
			break;
		default:
			break;
	}
}


void Zumi::zumiMove(int x, int y, int & decider)
{
	


	if (continueThatDirection == false)
	{
		int currentDirection = rand() % 4;

		switch(currentDirection)
		{
		case 0:
			decider = 0;
			break;
		case 1:
			decider = 1;
			break;
		case 2:
			decider = 2;
			break;
		case 3:
			decider = 3;
			break;
		}
		continueThatDirection = true;
	}
	else 
	{
		switch(decider)
		{
			case 0:
				moveTo(x + 1, y);			//Move right
				break;
			case 1:
				moveTo(x - 1, y);			//move left
				break;
			case 2:
				moveTo(x, y + 1);			//move up
				break;
			case 3:
				moveTo(x, y - 1);			//move down
				break;
		}
	}


}



void SimpleZumi::doSomething()
{
	//During a tick
	//1.
	if (isAlive() == false)				//if dead, return immediately.
		return;


	//3. decide to move? maybe have a tick counter. every x ticks = move.
	
	decreaseTicksOrReset();				//Tick counter?



	if (returnTicksPerMove() == 0)				//When ticks go to zero (so that it does this one every N ticks)
	{
		deadEnd(decider);
		//move
		zumiMove(getX(), getY(), decider);

		
	}

	//4.IF it did move? move in currentDirection if not occupied by sprayer or brick
		//Also, if destination contains brick or w/e, pick new currentDirection after waiting 
		//the amount of ticks in the counter (basically reset the counter)

	//When killed by bug spray
	if (getWorld()->WillDie(getX(), getY()))
	{
		//1.Set dead
		setDead();	
		//2. play sound of enemy dying
		getWorld()->playSound(SOUND_ENEMY_DIE);
		//3. Increase score by 100
		getWorld()->increaseScore(100);
		//4. determine chance of dropping a goodie
			//5. generate number between 0-99 inclusive . if less than prob generated, zumi drops goodie
		int x = rand() % 100;							//Generate random int from 0 through 99.
		if (x < getWorld()->getGoodieProb())
		{
			getWorld()->chooseGoodieToDrop(getX(), getY());
		}

		//6. decide which goodie to drop using StudentWorld for prob (page 40)
		//7. add to new location! :)D
		
		
		//Play the reveal exit sound only when this zumi's the last one (since its death means victory!)
		if (getWorld()->numofZumi() == 1)
			getWorld()->playSound(SOUND_REVEAL_EXIT);
	}

}


void ComplexZumi::doSomething()
{
	//During a tick
	//1.
	if (isAlive() == false)				//if dead, return immediately.
		return;
	//2.

	//3.	
	decreaseTicksOrReset();				//Tick counter?



	if (returnTicksPerMove() == 0)				//When ticks go to zero (so that it does this one every N ticks)
	{
		if (withinRange() == true)
		{
			
		
		}

		else
		{
		deadEnd(decider);
		//move
		zumiMove(getX(), getY(), decider);
		}
		
	}


	//4. If killed.
	if (getWorld()->WillDie(getX(), getY()))
	{
		//1.Set dead
		setDead();	
		//2. play sound of enemy dying
		getWorld()->playSound(SOUND_ENEMY_DIE);
		//3. Increase score by 500
		getWorld()->increaseScore(500);
		//4. determine chance of dropping a goodie
			//5. generate number between 0-99 inclusive . if less than prob generated, zumi drops goodie
		int x = rand() % 100;							//Generate random int from 0 through 99.
		if (x < getWorld()->getGoodieProb())
		{
			getWorld()->chooseGoodieToDrop(getX(), getY());
		}

		//6. decide which goodie to drop using StudentWorld for prob (page 40)
		//7. add to new location! :)D
		
		
		//Play the reveal exit sound only when this zumi's the last one (since its death means victory!)
		if (getWorld()->numofZumi() == 1)
			getWorld()->playSound(SOUND_REVEAL_EXIT);
	}

	//When killed by bug spray

}

int ComplexZumi::search()
{
	queue<Coord> coordQueue;
	//What the how to designate a coordinate as searched?
	//Maybe make a string maze thing...
	coordQueue.push(Coord(getX(), getY()));
	while (!coordQueue.empty())
	{
		Coord front = coordQueue.front();
		coordQueue.pop();


		return 0;

	}
	return -1;

}


int AbsValue(int target)
{
	int k;
	if (target < 0)
	{
		k = -1 * target;
		return k;
	}

	return target;

}	//returns absolute value of whatever is passed in

bool ComplexZumi::withinRange() const
{
	int playerX = getWorld()->getPlayer()->getX();
	int	playerY = getWorld()->getPlayer()->getY();
	int horizDist = AbsValue(playerX - getX());
	int vertDist = AbsValue(playerY - getY());
	if (horizDist <= getWorld()->smellDist() && vertDist <= getWorld()->smellDist())
		return true;
	else return false;

}


void Exit::doSomething()
{
	if (getWorld()->numofZumi() == 0)					//All zumi are defeated!
	{
	setVisible(true);
	}
	//StudentWorld does the rest of the Exit's function (apply bonus, finish level, play sound when player goes through).

	return;
}

void TimedLifeActor::expireImmediately()
{
	lifeTime = 0;
}

void TimedLifeActor::decrementLifetime()
{
	lifeTime--;
}

unsigned int TimedLifeActor::getLifetime() const
{
	return lifeTime;
}

void BugSprayer::doSomething()
{
	//1.
	if (isAlive() == false)						//if dead
		return;
	//2.
	decrementLifetime();						//Decrement lifetime by one

	if (getWorld()->WillDie(getX(), getY()) == true)
	{
		expireImmediately();
	}


	//3. If lifetime reaches zero, activate:
	if (getLifetime() == 0)
	{

		//a. adds a bug spray object onto its own square
		getWorld()->tryDroppingSprayAtAndIndicateContinuable(getX(),getY());

		//b. adds bug spray objects at up to 2 squares in each of the up/down/left/right
		if (getWorld()->tryDroppingSprayAtAndIndicateContinuable(getX() + 1,getY()) == true)
				getWorld()->tryDroppingSprayAtAndIndicateContinuable(getX() + 2,getY());

		if (getWorld()->tryDroppingSprayAtAndIndicateContinuable(getX() - 1,getY()) == true)				
				getWorld()->tryDroppingSprayAtAndIndicateContinuable(getX() - 2,getY());

		if (getWorld()->tryDroppingSprayAtAndIndicateContinuable(getX(),getY() + 1) == true)
				getWorld()->tryDroppingSprayAtAndIndicateContinuable(getX(),getY() + 2);

		if (getWorld()->tryDroppingSprayAtAndIndicateContinuable(getX(),getY() - 1) == true)				
				getWorld()->tryDroppingSprayAtAndIndicateContinuable(getX(),getY() - 2);
		//to form a + shape. NOT spread past brick, but can settle on destroyable brick and dissolve

		//c. Sound_spray sound effect played
		getWorld()->playSound(SOUND_SPRAY);

		//d. set the bugsprayer state to dead. 
		setDead();
	}

	return;
}

void BugSpray::doSomething()
{
	//1.
	if (isAlive() == false)
		return;
	//2.
	decrementLifetime();
	//3.
	if (getLifetime() == 0)
	{
		setDead();
	}

}


void ExtraLifeGoodie::doSomething()
{
	//1. if dead, do nothing and return.
	if (isAlive() == false)
		return;
	//2. decrement
	decrementLifetime();
	//3. If lifetime = 0, set to dead
	if (getLifetime() == 0)
	{
		setDead();
	}
	//4. Otherwise...
	//a. if the goodie is on the same square as any other object other than the player, no impact.
	//b. same square as player?
	if (getWorld()->colocatedGoodie(getX(),getY()))
	{
		//i. inform studentWorld that player got 1 extra life.
		getWorld()->incLives();
		//ii. inform studentworld that the user receives 1000 extra points (increaseScore()) method).
		getWorld()->increaseScore(1000);
		
		//iii. studentworld must set the goodie to dead (removed from the game at the end of the tick)
		setDead();
	
		///iv. playSound(SOUNT_GOT_GOODIE)
		getWorld()->playSound(SOUND_GOT_GOODIE);

	}
}

void WalkThroughWallsGoodie::doSomething()
{
	//1. if dead, do nothing and return.
	if (isAlive() == false)
		return;
	//2. decrement
	decrementLifetime();
	//3. If lifetime = 0, set to dead
	if (getLifetime() == 0)
	{
		setDead();
	}
	//4. Otherwise...
	//a. if the goodie is on the same square as any other object other than the player, no impact.
	//b. same square as player?
	if (getWorld()->colocatedGoodie(getX(),getY()))
	{
		//i. inform studentworld that the player can move through walls!!!
		getWorld()->ChangeActivationStatus();
		//ii. inform studentworld that the user receives 1000 extra points (increaseScore()) method).
		getWorld()->increaseScore(1000);
		
		//iii. studentworld must set the goodie to dead (removed from the game at the end of the tick)
		
		setDead();
	
		///iv. playSound(SOUNT_GOT_GOODIE)
		getWorld()->playSound(SOUND_GOT_GOODIE);

	}

}

void IncreaseSimultaneousSprayersGoodie::doSomething()
{
	//1. if dead, do nothing and return.
	if (isAlive() == false)
		return;
	//2. decrement
	decrementLifetime();
	//3. If lifetime = 0, set to dead
	if (getLifetime() == 0)
	{
		setDead();
	}

	//a. if the goodie is on the same square as any other object other than the player, no impact.
	//b. same square as player?
	if (getWorld()->colocatedGoodie(getX(),getY()))
	{
		//i. inform studentworld that the player can drop larger bug sprayers at a time (for ticks lifetime)
		getWorld()->changeActivateSprayers();

		//ii. inform studentworld that the user receives 1000 extra points (increaseScore()) method).
		getWorld()->increaseScore(1000);
		
		//iii. studentworld must set the goodie to dead (removed from the game at the end of the tick)
		
		setDead();
	
		///iv. playSound(SOUNT_GOT_GOODIE)
		getWorld()->playSound(SOUND_GOT_GOODIE);
	}
}

