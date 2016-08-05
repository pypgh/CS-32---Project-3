#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include "Level.h"
#include "Actor.h"
#include <vector>
using namespace std;

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
	StudentWorld()
	{
		//initialize data members
		level = 0;
		levelFinish = false;
	}

	~StudentWorld();

	virtual int init();

	virtual int move();

	virtual void cleanUp();

	void setDisplayText();

	bool willBeAtSamePlace(int nextX, int nextY) const;
	bool colocatedDead(int X, int Y) const;
	bool didZumiKill(int X, int Y) const;
	Player* getPlayer() const;
	bool colocatedExit(int X, int Y) const;
	void activateOrDeactivatePlayerWalkThroughWalls();
    bool playerCanWalkThroughWalls() const;
	void increaseorDecreasePlayerSimultaneousSprayers(int sprayernum);
	void addSprayerIfPossible(int x, int y);
	bool zumiAndSprayersBlockedAt(int x, int y) const;
	bool tryDroppingSprayAtAndIndicateContinuable(int x, int y);
	bool WillDie(int X, int Y) const;
	void dropSprayer(int x, int y);
	int numOfSprayersOut() const;
	bool zumiBlockedBySprayer(int nextX, int nextY) const;
	int numofZumi() const;
	int getGoodieProb() const;
	void chooseGoodieToDrop(int x, int y);
	bool colocatedGoodie(int x, int y) const;
	void ChangeActivationStatus();
	bool ZumiBLocked(int nextX, int nextY) const;
	void changeActivateSprayers();
	int getMaxSprayers() const;
	int smellDist() const;

	string returnLev(int k);

private:
	Player* m_player;
	vector<Actor*> m_actors;

	vector<DestrBrick*> m_destroyable;				//Have destroyable bricks in a vector since we're not sure how many there are.
	vector<PermaBrick*> m_perm;						//Similar idea with the perma bricks.
	
	int bonus;							//Bonus
	int goodieProb;						//General goodie probability
	int extraLifeProb;					//Extra life probability
	int	walkThruProb;					//Walk thru prob
	int	moreSprayersProb;				//more sprayers prob
	int goodieLifetime;					//Goodie lifetime
	int maxSprayersLifeTime;			//more sprayers life
	int walkThruLife;					//Walk thru lifetime
	int maxBoostedSprayers;				//Max boosted as loaded from level.

	bool ActivationStatus;
	int tempWalkThruLife;
	bool yes;

	bool ActivateExtraSprayers;
	bool yes2;
	int tempMaxSprayerLife;
	int level;
	bool levelFinish;
	int complexSmell;

	Level *Lev;
};

#endif // STUDENTWORLD_H_
