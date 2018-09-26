#include <sstream>
#include <string>

#include "worm.h"

using namespace std;

void Worm::gainLife()
{ lives++;
  // TODO: Alert the user of the new life.
}

void Worm::die(Game *game)
{ if(lives==0)
    alive=false;
  else
    lives--;

  x=startCol;
  y=startRow;
  game->reset();
}

void Worm::addScore(Game *game, int points)
{ if(points>=0 || score>=(unsigned)-points)
    score+=points;
  else
  { score=0;
    die(game);
  }

  residual+=points;

  while(residual>=3000)
  { residual-=3000;
    gainLife();
  }
}

bool Worm::clock(Game *game)
{ unsigned oldX=x,
           oldY=y;
  bool moved=false;

  // Check whether can do anything
  if(!alive)
    return false;

  // Check for anything active where we were.
  checkin(game);
  if(jumpstate==7) jumpstate=6;

  // Are we falling?
  if(!jumpstate && !game->hasClimbable(this, (int)y, (int)x)
     && !game->hasSolid(this, (int)y+1, (int)x))
    y++;
  else
    // Can we legally carry out the command?
    if(!ifLegal(game,game->getCommand()))
      if(!ifLegal(game,dir))
        ifLegal(game,STOP);

  // Check for anything active where we are.
  if(oldX!=x || oldY!=y)
  { checkout(game, oldX!=x && oldY==y, oldX);
    moved=true;
  }

  if(jumpstate) jumpstate--;

  return moved;
}

/* Check to see if there is anything active at our location. */
void Worm::checkin(Game *game)
{ game->touch(this);

  if(game->hasSpringy(this, y, x))
  { jumpstate=7;
    jumpbase=y;
  } else
    if(game->hasClimbable(this, y, x) && jumpstate)
    { jumpstate=0;
      dir=STOP;
      game->setCommand(STOP);
    }
}

/* Check to see if there is anything active at our location. */
void Worm::checkout(Game *game, bool stepOff, unsigned oldX)
{ if(stepOff) game->stepOff(this, oldX);
  game->touch(this);

  if(jumpstate)
    for(unsigned i=jumpbase; i>y; i--)
      game->jumpOver(this, (int)i);

  if(game->hasSpringy(this, y, x))
  { jumpstate=7;
    jumpbase=y;
  } else
    if(game->hasClimbable(this, y, x) && jumpstate)
    { jumpstate=0;
      dir=STOP;
      game->setCommand(STOP);
    }
}

/* Try to carry out a command.
 * Return true if it was possible, false otherwise.
 */
bool Worm::ifLegal(Game *game, Command command)
{ int nextY=y;

  if(jumpstate)
  { int ideal=jumpbase;

    if(jumpstate>3)
      ideal-=7-jumpstate;
    else
      ideal-=jumpstate-1;

    if(ideal>(int)y) nextY++;
    else if(ideal<(int)y) nextY--;
  }
  
  // Check 
    
  switch(command)
  { case LEFT:
      if(!game->hasSolid(this, nextY, (int)x-1))
      { dir=command;
        y=nextY;
	x--;
	return true;
      }
      return false;
      
    case RIGHT:
      if(!game->hasSolid(this, nextY, (int)x+1))
      { dir=command;
        y=nextY;
	x++;
	return true;
      }
      return false;

    case UP:
      if(game->hasClimbable(this, (int)y, (int)x)
         && game->hasClimbable(this, (int)y-1, (int)x))
      { dir=command;
	y--;
	return true;
      }
      return false;

    case DOWN:
      if(game->hasClimbable(this, (int)y+1, (int)x))
      { dir=DOWN;
	y++;
	return true;
      }
      return false;

    case JUMP:
      if(!jumpstate && game->hasSolid(this, (int)y+1, (int)x))
      { jumpstate=6;
        jumpbase=y;
      }
      return false; // We started the jump, someone else must do it.

    case DIE:
      die(game);
      return true;

    default:
      if(!game->hasSolid(this, (int)nextY, (int)x))
      { dir=STOP;
        y=nextY;
	return true;
      }
      return false;
  }
}

string Worm::getStatus()
{ ostringstream statline;

  statline << "Worms ";
  if(lives<6)
  { for(unsigned i=0; i<lives; i++)  
      statline << '&';
    for(unsigned i=lives; i<5; i++)
      statline << ' ';
  } else
    statline << "&&&&&+";

  statline << "  Score " << score << "  ";
  return statline.str();
}
