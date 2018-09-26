#include <time.h>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <list>
#include <sstream>
#include <string>

#include <cstring>

#include "game.h"
#include "worm.h"

using namespace std;

// Macro to access the contents by row and column
#define LEVEL(r,c) (this->level[(c)*(this->rows)+(r)])

#ifndef BONUSFRAMES
#define BONUSFRAMES 14
#endif

#ifndef FRAMELEN
#define FRAMELEN (1500000000/BONUSFRAMES)
#endif


GameElement::GameElement(const chtype name) : name(name) {}
GameElement::GameElement() : name(0) {}

#define FIXACS(c,code) ((code)?(code):(c))
Game::Game(const char *map, GameDisplay *display) :
  rows(24), columns(40), startRow(0), startCol(0), display(display)
{   list<string> contents;
    ifstream file(map,ios::binary);
    std::unique_ptr<char[]> page(new char[rows*columns]);

    init();

    for(int i=0; i<128; i++)
        catalog[i]=new GameElement(i);

    catalog[128+0]=new GameElement(128+127);
    catalog[128+1]=new GameElement(128+127);
    
    for(int i=128+2; i<128+51; i++)
		catalog[i] = new GameElement(i);

	catalog[130] = new Present(130); //Symbol '#'. 
	catalog[133] = new Spring(133); //Symbol 'Z'.
	catalog[131] = new Ladder(131); //Symbol 'H'.
	
    for(int i=128+51; i<128+91; i++)
        catalog[i]=new SolidGameElement(i);
        
    for(int i=128+91; i<128+128; i++)
        catalog[i]=new GameElement(i);

    // Read the level from the file.
    while(file.read(page.get(), rows*columns))
		levels.push_back(GameLevel(rows, columns, page.get(), catalog));
	
    for(int l=0; l<levels.size(); l++)
        levels[l].setWorm(page[2*l+1]-1,page[2*l]-1);
        
    level.reset(new std::list<GameElement *>[rows*columns]);
    display->setGame(this, rows, columns);
    switchLevel(0);

    reset();
    showStatus();
    display->clock(this);
}

Game::~Game()
{ 
    for(int i=0; i<256; i++)
        delete(catalog[i]);

    for(list<GameAgent *>::iterator i=agents.begin();
            i!=agents.end();
            i++)
        delete(*i);
}

void Game::reset()
{ bonus=101*BONUSFRAMES;
}

int Game::getBonus()
{ return (bonus/BONUSFRAMES)*10;
}

bool Game::clock()
{ int y, x;
  
  command=display->getCommand();

  display->writeAt(rows,50,commandName());

  if(command==QUIT)
    return false;

  if(bonus--==0)
  { y=willy->getRow();
    x=willy->getCol();
    willy->die(this);
    LEVEL(willy->getRow(), willy->getCol()).push_back(willy);
    LEVEL(y, x).remove(willy);
    willy->draw(display);
    LEVEL(y, x).back()->draw(display, y, x);
  }

  for(list<GameAgent *>::iterator i=agents.begin();
      i!=agents.end();
      i++)
  { y=(*i)->getRow();
    x=(*i)->getCol();
    if((*i)->clock(this))
    { LEVEL((*i)->getRow(), (*i)->getCol()).push_back(*i);
      LEVEL(y, x).remove(*i);
      (*i)->draw(display);
      LEVEL(y, x).back()->draw(display, y, x);
    }
  }

  display->center(willy->getRow(), willy->getCol(), 10, 1);
  showStatus();
  display->clock(this);
  return willy->isAlive();
}

void Game::showStatus()
{ ostringstream statline;

  statline << "  Bonus ";
  statline.width(4);
  statline << getBonus();
  statline.width(1);
  statline << "  ";
  statline << willy->getStatus();
  display->writeAt(rows, 0, statline.str());
}

bool Game::hasSolid(GameAgent *agent, int row, int col)
{ ActiveGameElement *a;

  if(!valid(row,col)) return true;

  list<GameElement *> &e=LEVEL(row, col);

  for(list<GameElement *>::iterator i=e.begin(); i!=e.end(); i++)
    if(*i!=agent && (a=dynamic_cast<ActiveGameElement *>(*i))
       && a->isSolid())
      return true;

  return false;
}
  
bool Game::hasSpringy(GameAgent *agent, int row, int col)
{ ActiveGameElement *a;

  if(!valid(row,col)) return true;

  list<GameElement *> &e=LEVEL(row, col);

  for(list<GameElement *>::iterator i=e.begin(); i!=e.end(); i++)
    if(*i!=agent && (a=dynamic_cast<ActiveGameElement *>(*i))
       && a->isSpringy())
      return true;

  return false;
}
  
bool Game::hasClimbable(GameAgent *agent, int row, int col)
{ ActiveGameElement *a;

  if(!valid(row,col)) return false;

  list<GameElement *> &e=LEVEL(row, col);

  for(list<GameElement *>::iterator i=e.begin(); i!=e.end(); i++)
    if(*i!=agent && (a=dynamic_cast<ActiveGameElement *>(*i))
       && a->isClimbable())
      return true;

  return false;
}
  
void Game::stepOff(GameAgent *agent, int col)
{ unsigned row=agent->getRow()+1;
  ActiveGameElement *a;

  if(!valid(row, col)) return;

  list<GameElement *> &e=LEVEL(row, col);

  for(list<GameElement *>::iterator i=e.begin(); i!=e.end(); i++)
    if(*i!=agent && (a=dynamic_cast<ActiveGameElement *>(*i)))
      a->stepOff(this, agent);
}
  

void Game::switchLevel(int new_level)
{   //if(levels) return;
    current_level=new_level%levels.size();
    GameLevel &l=levels[current_level];
    startRow=l.getWormRow();
    startCol=l.getWormColumn();
    willy=new Worm(128, startRow, startCol);
    
    for(GameElement *elem: agents)
        delete(elem);
    agents.clear();

    agents.push_back(willy);

    for(unsigned i=0; i<rows*columns; i++)
    {   unsigned r,c;
        l.getIndex(i, r, c);
        level[i].clear();
        GameElement *e=l.getLevel()[i];
        level[i].push_back(e);
        e->draw(display, r, c);
    }
    
    level[l.getIndex(startRow, startCol)].push_back(willy);
    willy->draw(display);
    display->center(startRow, startCol, 10, 1);
}

void Game::touch(GameAgent *agent)
{ unsigned row=agent->getRow(),
           col=agent->getCol();
  ActiveGameElement *a;

  if(!valid(row, col)) return;

  list<GameElement *> &e=LEVEL(row, col);

  for(list<GameElement *>::iterator i=e.begin(); i!=e.end(); i++)
    if(*i!=agent && (a=dynamic_cast<ActiveGameElement *>(*i)))
      a->touch(this, agent);
}
  
void Game::jumpOver(GameAgent *agent, int row)
{ unsigned col=agent->getCol();
  ActiveGameElement *a;

  if(!valid(row, col)) return;

  list<GameElement *> &e=LEVEL(row, col);

  for(list<GameElement *>::iterator i=e.begin(); i!=e.end(); i++)
    if(*i!=agent && (a=dynamic_cast<ActiveGameElement *>(*i)))
      a->jumpOver(this, agent);
}

void Game::start()
{ timespec nextcycle, remain;
  long frame=FRAMELEN, slop=frame/10;
  //ostringstream msg;
  
  // Initialize a timer that can accurately report very small time diffs.
  clock_gettime(CLOCK_MONOTONIC, &nextcycle);

  for(;;)
  { int stat=-1;
  
    //display->writeAt(rows, 10, "clk");

    if(!clock())
      return;

    nextcycle.tv_nsec+=frame;
    remain.tv_nsec=frame;

    //display->writeAt(rows, 10, "adv");
    while(nextcycle.tv_nsec>=999999999)
    { nextcycle.tv_sec++;
      nextcycle.tv_nsec-=1000000000;
    }

    //msg.str("sleep");
    //msg << nextcycle.tv_sec << "." << nextcycle.tv_nsec;
    //display->writeAt(rows, 20, msg.str());
    while(stat<0 && remain.tv_nsec>slop)
      stat=clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME,
                           &nextcycle, &remain);
  }
}

/* Replace a GameElement at a given location.
 * Input:
 *   row, col: location
 *   e: GameElement to replace
 *   name: name of replacement element
 */
void Game::replace(unsigned row, unsigned col, GameElement *old, chtype c)
{ // Find the new element in the catalog
  GameElement *e=catalog[(unsigned char)c];

  // Find the list for this location in the level
  list<GameElement *>& location=LEVEL(row, col);

  // Replace the element in the list.
  std::replace(location.begin(), location.end(), old, e);

  // Update the display
  // TODO: broken! this code is only valid in the constructor....
  e->draw(display, row, col);
}

/*
int main(int argc, char *argv[])
{ Game *game=new Game(argc>1?argv[1]:"willy1.txt");

  game->start();
  delete(game);
}
*/