#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include "GameConstants.h"
#include "Gameworld.h"
//#include "StudentWorld.h"

class StudentWorld;

class Actor: public GraphObject
{
	public:
		Actor(int imageID, int startX, int startY, StudentWorld* wld)
			:GraphObject(imageID, startX, startY)
		{
			setVisible(true);
			m_world = wld;
			m_alive = true;
			canBeWalkedThrough = false;
		}

		virtual void doSomething() = 0;

		StudentWorld* getWorld() const;

		bool isAlive() const;											//implement? IS this actor alive?

		void setDead();													//Mark as dead

		void changeWalkThrough();										//Change walk through property.
		
		bool walkThrough() const;

		bool sprayBlocksZumi(int nextX, int nextY) const;
	
		virtual ~Actor()
		{
		}
	private:
		bool m_alive;
		StudentWorld *m_world;	
		int m_ID;
		bool canBeWalkedThrough;
		//How to initialize this?? or whatever?
};

class Player: public Actor
{
	public:
		Player(int startX, int startY, StudentWorld* wld)
			:Actor(IID_PLAYER, startX, startY, wld)
		{
			num_sprayers = 2;
			orig_sprayers_num = 2;
		}
		virtual void doSomething();								//Got to make a non pure function for Player.

		void increaseSimultaneousSprayers(unsigned int max, unsigned int lifetime);
		int OrigNumSprayers();

	private:
		int num_sprayers;
		int orig_sprayers_num;

};



class Brick: public Actor
{
	public:
		Brick(int imageID, int startX, int startY, StudentWorld* wld)
			:Actor(imageID, startX, startY, wld)
		{}
	private:

};

class PermaBrick: public Brick
{
	public:
		PermaBrick(int startX, int startY, StudentWorld* wld)
			:Brick(IID_PERMA_BRICK, startX, startY, wld)
		{
		}

		virtual void doSomething()
		{}

	private:

};

class DestrBrick: public Brick
{
	public:
		DestrBrick(int startX, int startY, StudentWorld* wld)
			:Brick(IID_DESTROYABLE_BRICK, startX, startY, wld)
		{}
		

	
		virtual void doSomething();

	private:

};



class Zumi: public Actor
{
	public:
		Zumi(int imageID, int startX, int startY, StudentWorld* wld, unsigned int ticksPerMove)
			:Actor(imageID, startX, startY, wld)
		{
			z_ticksPerMove = ticksPerMove;
			constTicks = ticksPerMove;
			continueThatDirection = false;

		}

		void deadEnd(int decider);
		void decreaseTicksOrReset();
		int returnTicksPerMove() const;
		void zumiMove(int x, int y, int & decider);
	private:
		bool continueThatDirection;
		int z_ticksPerMove;
		int constTicks;

};

class SimpleZumi: public Zumi
{
	public:
		SimpleZumi(int startX, int startY, StudentWorld* wld, unsigned int ticksPerMove)
			:Zumi(IID_SIMPLE_ZUMI, startX, startY, wld, ticksPerMove)
		{
			decider = -1;
		}
		virtual void doSomething();
	private:
		int decider;
	
};

class ComplexZumi: public Zumi
{
	public:
		ComplexZumi(int startX, int startY, StudentWorld* wld, unsigned int ticksPerMove)
			:Zumi(IID_COMPLEX_ZUMI, startX, startY, wld, ticksPerMove)
		{
			decider = -1;
		}
		virtual void doSomething();
		bool withinRange() const;
		int search();
	private:
		int decider;
};

class Coord
{
	public:
		Coord(int x, int y) 
			:m_x(x), m_y(y) 
		{}
		int x() const
		{
			return m_x;
		}
		int y() const
		{
			return m_y;
		}
	private:
		int m_x;
		int m_y;

};

class Exit: public Actor
{
	public:
		Exit(int startX, int startY, StudentWorld* wld)
			:Actor(IID_EXIT, startX, startY, wld)
		{
			setVisible(false);								//Initially have it invisible. 
			visible = false;
		}
		virtual void doSomething();
	private:
		bool visible;

};


class TimedLifeActor: public Actor
{
	public:
		TimedLifeActor(int imageID, int startX, int startY, StudentWorld* wld, unsigned int givenLifeTime)
			:Actor(imageID, startX, startY, wld)
		{
			lifeTime = givenLifeTime;
		}
		unsigned int getLifetime() const;
		void decrementLifetime();
		  // Set remining lifetime to 0
		void expireImmediately();
	private:
		unsigned int lifeTime;
};



class BugSprayer: public TimedLifeActor
{
	public:
		BugSprayer(int startX, int startY, StudentWorld* wld)
			:TimedLifeActor(IID_BUGSPRAYER, startX, startY, wld, 40)		//construct bugsprayer, appropriate ID, lifetime.
		{}
		virtual void doSomething();


		//Implement later
		/*virtual void applyBugSpray();
		virtual bool blocksZumiAndSprayers() const;*/

};

class BugSpray: public TimedLifeActor
{
	public:
		BugSpray(int startX, int startY, StudentWorld* wld)
			:TimedLifeActor(IID_BUGSPRAY, startX, startY, wld, 3)		//3 ticks lifetime, bugspray id, xy location.
		{}
		virtual void doSomething();

};

class Goodie: public TimedLifeActor
{
	public:
		Goodie(int imageID, int startX, int startY, StudentWorld* wld, unsigned int lifeTime)
			:TimedLifeActor(imageID, startX, startY, wld, lifeTime)
		{}


};


class ExtraLifeGoodie: public Goodie
{
	public:
		ExtraLifeGoodie(int startX, int startY, StudentWorld* wld, unsigned int lifetime)
			:Goodie(IID_EXTRA_LIFE_GOODIE, startX, startY, wld, lifetime)
		{}
		virtual void doSomething(); 
	private:

};

class WalkThroughWallsGoodie: public Goodie
{
	public:
		WalkThroughWallsGoodie(int startX, int startY, StudentWorld* wld, unsigned int lifetime)
			:Goodie(IID_WALK_THRU_GOODIE, startX, startY, wld, lifetime)
		{}
		virtual void doSomething();
	private:

};

class IncreaseSimultaneousSprayersGoodie: public Goodie
{
	public:
		IncreaseSimultaneousSprayersGoodie(int startX, int startY, StudentWorld* wld, unsigned int lifetime)
			:Goodie(IID_INCREASE_SIMULTANEOUS_SPRAYER_GOODIE, startX, startY, wld, lifetime)
		{}
		virtual void doSomething();
	private:

};

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

#endif // ACTOR_H_
