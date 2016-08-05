#include "StudentWorld.h"

///OMG CHECK PAGE 14 FOR LEVEL DETAILS!!!


GameWorld* createStudentWorld()
{
	return new StudentWorld();
}

StudentWorld::~StudentWorld()
{
		//delete m_player;?????
		delete m_player;

		vector<Actor*>::iterator it;
		for (it = m_actors.begin(); it!= m_actors.end();)
		{
			delete *it;
			it = m_actors.erase(it);
		}
}

int StudentWorld::init()
{
		Lev = new Level();
		Level::LoadResult result = Lev->loadLevel(returnLev(level));				//Loads level
		if (result == Level::load_fail_file_not_found && level == 0)
			return GWSTATUS_NO_FIRST_LEVEL;										//Error message if level not found.
		if (result == Level::load_fail_file_not_found && level > 0)
			return GWSTATUS_PLAYER_WON;


		for (int i = 0; i < VIEW_HEIGHT; i++)									//Scan all items in the level
		{
			for (int k = 0; k < VIEW_WIDTH; k++)
			{
				if (Lev->getContentsOf(i,k) == Level::player)					//Correspond to player? Initialize a dynamic player.
				{
					m_player = new Player(i, k, this);
				}
				if (Lev->getContentsOf(i,k) == Level::perma_brick)				//Brick? Add a new brick dynamically to the vector.
				{
					m_actors.push_back(new PermaBrick(i, k, this)); 
				}
				if (Lev->getContentsOf(i,k) == Level::destroyable_brick)		//Do to the rest of the members that are there in the beginning of the level.
				{
					m_actors.push_back(new DestrBrick(i, k, this));
				}
				if (Lev->getContentsOf(i,k) == Level::simple_zumi)				
				{
					m_actors.push_back(new SimpleZumi(i, k, this, Lev->getOptionValue(optionTicksPerSimpleZumiMove)));
				}
				if (Lev->getContentsOf(i,k) == Level::complex_zumi)
				{
					m_actors.push_back(new ComplexZumi(i, k, this, Lev->getOptionValue(optionTicksPerComplexZumiMove)));
				}
				if (Lev->getContentsOf(i,k) == Level::exit)
				{
					m_actors.push_back(new Exit(i, k, this));
				}
				//Add others?
			}	
		}




		bonus = Lev->getOptionValue(optionLevelBonus);									//Get bonus from the level 
		goodieProb = Lev->getOptionValue(optionProbOfGoodieOverall);						//Get overall probability of the goodies.
		extraLifeProb = Lev->getOptionValue(optionProbOfExtraLifeGoodie);				//Extra life prob
		walkThruProb = Lev->getOptionValue(optionProbOfWalkThruGoodie);					//Walk thru prob
		moreSprayersProb = Lev->getOptionValue(optionProbOfMoreSprayersGoodie);			//more sprayers prob
		goodieLifetime = Lev->getOptionValue(optionGoodieLifetimeInTicks);				//Goodie lifetime
		maxSprayersLifeTime = Lev->getOptionValue(optionBoostedSprayerLifetimeTicks);	//more sprayers life
		walkThruLife = Lev->getOptionValue(optionWalkThruLifetimeTicks);					//Walk thru lifetime
		maxBoostedSprayers = Lev->getOptionValue(optionMaxBoostedSprayers);				//Max boosted as loaded from level.
		ActivationStatus = false;												//Default so we don't activate walktHru
		ActivateExtraSprayers = false;											//Default so we don't activate.
		tempWalkThruLife = Lev->getOptionValue(optionWalkThruLifetimeTicks);		//Assign temp for walkthru usage.
		tempMaxSprayerLife = Lev->getOptionValue(optionBoostedSprayerLifetimeTicks);	//Temp for boosted sprayer
		yes = true;
		yes2 = true;
		complexSmell = Lev->getOptionValue(optionComplexZumiSearchDistance);
		return GWSTATUS_CONTINUE_GAME;


}


int StudentWorld::move()
{
		m_player->doSomething();
		for (size_t k = 0; k < m_actors.size(); k++)					//Make all actors do something.
			m_actors[k]->doSomething();
		if (m_player->isAlive() == false)								//If player's dead, return status.
		{
			decLives();
			return GWSTATUS_PLAYER_DIED;		
		}

		if (bonus > 0)
			bonus--;
		//Decrease the bonus? Where is the bonus?
		
		//Responsible for activating the Exit object on the current level so that it becomes visible: invisible/inactive->visible/active
		//How to remove stuff? Etc etc etc etc etc etc etc etc etc etc etc . 
				//Maybe if isDead is true, we can delete or whatever...


		if (ActivateExtraSprayers == true)						//When we step on the walkThruWalls goodie, this will become true
		{
			if (yes2 == true)									//Use a bool to control how many times this is activated
			{
				increaseorDecreasePlayerSimultaneousSprayers(maxBoostedSprayers);	//Increase to max boosted.
				yes2 = false;														//Doesn't go through this again.
			}
			if (tempMaxSprayerLife > 0)
			tempMaxSprayerLife--;

			if (tempMaxSprayerLife == 0)							//When the goodie's effect life is up
			{
				changeActivateSprayers();						//change activation status to false so it doesn't go through this again.
				increaseorDecreasePlayerSimultaneousSprayers(m_player->OrigNumSprayers());	//deactivate the effect.
				tempMaxSprayerLife = maxSprayersLifeTime;				//Change the temp back in case we get a power up again.
				yes2 = true;										//Change this bool back so we can activate it again if we go through this if statement.
			}

		}




		if (ActivationStatus == true)						//When we step on the walkThruWalls goodie, this will become true
		{
			if (yes == true)									//Use a bool to control how many times this is activated
			{
			activateOrDeactivatePlayerWalkThroughWalls();
			yes = false;
			}
			if (tempWalkThruLife > 0)
			tempWalkThruLife--;

			if (tempWalkThruLife == 0)							//When the goodie's effect life is up
			{
				ChangeActivationStatus();						//change activation status to false so it doesn't go through this again.
				activateOrDeactivatePlayerWalkThroughWalls();	//deactivate the effect.
				tempWalkThruLife = walkThruLife;				//Change the temp back in case we get a power up again.
				yes = true;										//Change this bool back so we can activate it again if we go through this if statement.
			}

		}
		
		for (unsigned int k = 0; k < m_actors.size(); k++)
		{
			if (m_actors[k]->getID() == IID_EXIT && colocatedExit(m_actors[k]->getX(), m_actors[k]->getY()))		// if Player is at exit.
				//m_actors[k]->finishLevel())
			{
					playSound(SOUND_FINISHED_LEVEL);							//Play the SOUND!
					increaseScore(bonus);										//Apply the BONUS!!!!! :o)
					levelFinish = true;
					return GWSTATUS_FINISHED_LEVEL;								//Notify level completion.
			}
		}

		setDisplayText();

		vector<Actor*>::iterator it;									//Dispose of all dead actors. LOL
		for (it = m_actors.begin(); it!= m_actors.end();)
		{ 
			if ((*it)->isAlive() == false)
			{
			delete *it;
			it = m_actors.erase(it);
			}
			else it++;
		}






		return GWSTATUS_CONTINUE_GAME;
		// This code is here merely to allow the game to build, run, and terminate after hitting enter a few times 
        //decLives();
        //return GWSTATUS_PLAYER_DIED;
}

void StudentWorld::setDisplayText()
{
	int score = getScore();
	int level = getLevel();
	int lives = getLives();

	string s = to_string(score);
	string lev = to_string(level);
	string liv = to_string(lives);
	string b = to_string(bonus);



	string stream = "Score: ";
	switch (s.size())
	{
		case 0:
			stream+="0000000";
			break;
		case 1:
			stream+="0000000";
			break;
		case 2:
			stream+="000000";
			break;
		case 3:
			stream+="00000";
			break;	
		case 4:
			stream+="0000";
			break;
		case 5:
			stream+="000";
			break;
		case 6:
			stream+="00";
			break;
		case 7:
			stream+="0";
			break;
		default:
			break;
	}

	stream += s;
	stream += "  Level: ";
	stream += lev;

	stream += "  Lives: ";
	stream += liv;

	stream += "  Bonus: ";
	switch(b.size())
	{
		case 1:
			stream+= "     ";
			break;
		case 2:
			stream+= "    ";
			break;
		case 3: 
			stream+= "   ";
			break;
		case 4:
			stream+= "  ";
			break;
		case 5: 
			stream+= " ";
		default:
			break;
	}
	stream += b;


	//s+=bonus;

	//s+=itoa(bonus, 
	setGameStatText(stream);

}


void StudentWorld::cleanUp()
{
	//Called when palyer  completes current level or loses a life.
		delete m_player;

		vector<Actor*>::iterator it;
		for (it = m_actors.begin(); it!= m_actors.end();)
		{
			delete *it;											//Delete dynamically allocated data using iterator
			it = m_actors.erase(it);
		}
		if (levelFinish == true)								//Since we finished the level, we go to the next level.
		{
			levelFinish = false;
			level++;
			delete Lev;
			Lev = nullptr;
		}
}

bool StudentWorld::willBeAtSamePlace(int nextX, int nextY) const			//Checks if the bricks are at this coordinate. 
																		//Used with the samePlace function in Actor.cpp
																		//For the situation that the next player's move is where a brick is.
{
	for (size_t i = 0; i < m_actors.size(); i++)
		{
			if (nextX == m_actors[i]->getX() && nextY == m_actors[i]->getY())
			{
				if(m_actors[i]->getID() == IID_PERMA_BRICK)
					return true;

				if (m_actors[i]->getID() == IID_DESTROYABLE_BRICK && m_actors[i]->walkThrough() == false)
					return true;
			}
		}
		return false;
}

bool StudentWorld::ZumiBLocked(int nextX, int nextY) const			//Checks if the bricks are at this coordinate. 
																		//Used with the samePlace function in Actor.cpp
																		//For the situation that the next zumi's move is where a brick is.
{
	for (size_t i = 0; i < m_actors.size(); i++)
		{
			if (nextX == m_actors[i]->getX() && nextY == m_actors[i]->getY())
			{
				if(m_actors[i]->getID() == IID_PERMA_BRICK)
					return true;

				if (m_actors[i]->getID() == IID_DESTROYABLE_BRICK)
					return true;
			}
		}
		return false;
}

bool StudentWorld::zumiBlockedBySprayer(int nextX, int nextY) const		//Checks if that space is occupied by a sprayer so that the zumi won't walk on it.
{
	for (size_t i = 0; i < m_actors.size(); i++)
		{
			if (nextX == m_actors[i]->getX() && nextY == m_actors[i]->getY())
			{
				if(m_actors[i]->getID() == IID_BUGSPRAYER)
					return true;
			}
		}
		return false;
}


bool StudentWorld::colocatedDead(int X, int Y) const										//Check if the player is at the same spot as a killer material.
{                                                                                           //if so, return true(other funcs will make player dead).
		for (size_t i = 0; i < m_actors.size(); i++)
		{
			if (X == m_actors[i]->getX() && Y == m_actors[i]->getY())
			{
				if (m_actors[i]->getID() == IID_DESTROYABLE_BRICK && m_actors[i]->walkThrough() == false)
				{
					return true;
				}
				if (m_actors[i]->getID() == IID_SIMPLE_ZUMI)
					return true;
				if (m_actors[i]->getID() == IID_COMPLEX_ZUMI)
					return true;
				if (m_actors[i]->getID() == IID_BUGSPRAY)
					return true;
					//Do for the rest of the deadly items (duh). I think that's it.
			}
		}
		return false;		
}

bool StudentWorld::WillDie(int X, int Y) const										//Check if the zumi is at the same spot as a killer material.
{                                                                                           //if so, return true
		for (size_t i = 0; i < m_actors.size(); i++)
		{
			if (X == m_actors[i]->getX() && Y == m_actors[i]->getY())
			{
				if (m_actors[i]->getID() == IID_BUGSPRAY)
					return true;
			}
		}
		return false;		
}

bool StudentWorld::colocatedExit(int X, int Y) const
{
	if (X == m_player->getX() && Y == m_player->getY())
		return true;
	else return false;
}

Player* StudentWorld::getPlayer() const
{
	return m_player;
}

void  StudentWorld::ChangeActivationStatus()
{
	if (ActivationStatus == true)
		ActivationStatus = false;
	else ActivationStatus = true;
}


void StudentWorld::activateOrDeactivatePlayerWalkThroughWalls()		//Call this once when the player gets the goodie,
																	//call once more when the goodie's effect expires.
{

	for (unsigned int i = 0; i < m_actors.size(); i++)
	{
		if (m_actors[i]->getID() == IID_DESTROYABLE_BRICK)
		{
			m_actors[i]->changeWalkThrough();				//If it's a destroyable brick, change the walkthrough properties.
		}
	}

}

//Can player walk across bricks?
bool StudentWorld::playerCanWalkThroughWalls() const
{
	for (unsigned int i = 0; i < m_actors.size(); i++)
	{
		if (m_actors[i]->getID() == IID_DESTROYABLE_BRICK && m_actors[i]->walkThrough() == true)
			return true;
	}
	return false;
}

//Drop a sprayer at this x,y location if it's possible.
void StudentWorld::addSprayerIfPossible(int x, int y)	//should call with only the player's GetX and GetY.
{
	for (unsigned int k = 0; k < m_actors.size(); k++)
	{
		if (m_actors[k]->getID() == IID_BUGSPRAYER &&					//Basically, if the player is colocated with a sprayer that is currently on
																		//the ground, then just return from the function.
			m_player->getX() == m_actors[k]->getX() && 
			m_player->getY() == m_actors[k]->getY())
			return;
		if (m_actors[k]->getID() == IID_DESTROYABLE_BRICK &&					//cannot drop on destr bricks
																	
			m_player->getX() == m_actors[k]->getX() && 
			m_player->getY() == m_actors[k]->getY())
			return;
	}

	m_actors.push_back(new BugSprayer(x, y, this));						//Construct a bugsprayer.
}

bool StudentWorld::zumiAndSprayersBlockedAt(int x, int y) const
{
	for (unsigned int k = 0; k < m_actors.size(); k++)
	{
		if (m_actors[k]->getX() == x && m_actors[k]->getY() == y)
		{
			if (m_actors[k]->getID() == IID_BUGSPRAYER ||
				m_actors[k]->getID() == IID_PERMA_BRICK ||
				m_actors[k]->getID() == IID_DESTROYABLE_BRICK)
			return true;			//Return that yes, it is blocked there.
		}
	}
	return false;
}


bool StudentWorld::tryDroppingSprayAtAndIndicateContinuable(int x, int y)
{
	for (unsigned int k = 0; k < m_actors.size(); k++)
	{
		if (m_actors[k]->getX() == x && m_actors[k]->getY() == y)
		{
			if (m_actors[k]->getID() == IID_PERMA_BRICK)
			return false;			//If it's a brick, then don't drop there!!
		}
	}
	m_actors.push_back(new BugSpray(x, y, this));
	for (unsigned int k = 0; k < m_actors.size(); k++)
	{
		if (m_actors[k]->getX() == x && m_actors[k]->getY() == y)
		{
			if (													//This allows the spray to be dropped here, but then check if it can continue...
				//m_actors[k]->getID() == IID_COMPLEX_ZUMI ||
				//m_actors[k]->getID() == IID_SIMPLE_ZUMI ||
				//m_actors[k]->getID() == IID_PERMA_BRICK ||
				m_actors[k]->getID() == IID_DESTROYABLE_BRICK)
			return false;			//If any of the actors at that coord. are items that block the spray from continuing,
							//do nothing and return.
		}
	}
	return true;
}

		
int StudentWorld::numOfSprayersOut() const								//Return the amount of sprayers that are present. 
{
	int count = 0;
	for (unsigned int k = 0; k < m_actors.size(); k++)
	{
		if (m_actors[k]->getID() == IID_BUGSPRAYER)
			count++;
	}
	return count;
}


int StudentWorld::numofZumi() const				//Count how many zumi remain (if zero, then level's finished, reveal exit [diff func]).
{
	int count = 0;
	for (unsigned int k = 0; k < m_actors.size(); k++)
	{
		if (m_actors[k]->getID() == IID_COMPLEX_ZUMI || m_actors[k]->getID() == IID_SIMPLE_ZUMI)
			count++;
	}
	return count;
}


int StudentWorld::getGoodieProb() const				// obtains the zumi's probability. 
{
	return goodieProb;
}

void StudentWorld:: chooseGoodieToDrop(int x, int y)			//Choose a goodie to drop.
{
	int p = rand() % 100;									//Since all probs add to 100
	if (p >= 0 && p < walkThruProb)							//This is 0 to walkThruProb -1 (walkThruProb amount of nums)
	{
		m_actors.push_back(new WalkThroughWallsGoodie(x, y, this, goodieLifetime));
		return;
	}
	if (p >= walkThruProb && p < walkThruProb + moreSprayersProb)		//Same here: there are moreSprayersProb amount of nums
	{
		m_actors.push_back(new IncreaseSimultaneousSprayersGoodie(x, y, this, goodieLifetime));
		return;
	}
	else																//Extra life takes up the rest of the nums.
		m_actors.push_back(new ExtraLifeGoodie(x, y, this, goodieLifetime));
	return;
}

bool StudentWorld::colocatedGoodie(int X, int Y) const			//colocated with goodie? Return true.
{
	if (X == m_player->getX() && Y == m_player->getY())
		return true;
	else return false;
}


//increase number of simulatneous sprayers that can be dropped
void StudentWorld::increaseorDecreasePlayerSimultaneousSprayers(int sprayernum)
{
	m_player->increaseSimultaneousSprayers(sprayernum, maxSprayersLifeTime);

}


void StudentWorld::changeActivateSprayers()		//Acts as a switch for the sprayer boost
{
	if (ActivateExtraSprayers == true)
		ActivateExtraSprayers = false;
	else ActivateExtraSprayers = true;
}


int StudentWorld::getMaxSprayers() const		//Returns max boosted sprayer count.
{
	return maxBoostedSprayers;
}

// Students:  Add code to this file (if you wish), StudentWorld.h, Actor.h and Actor.cpp

string StudentWorld::returnLev(int k)			//Returns level name so that I could vary the levels initialized in init()
{
	string s = "level";
	string levNum = to_string(k);
	if (levNum.size() == 1)
	{
		s+="0";
		s+=levNum;
	}
	if (levNum.size() > 1)
		s+=levNum;
	s+= ".dat";
	return s;

}

int StudentWorld::smellDist() const		//Returns the smell distance for the actor class to use. 
{
	return complexSmell;
}

