#include "game.h"

#ifndef WILLY_H
#define WILLY_H

/* This is the implementation of a worm agent. */
class Worm : public GameAgent
{ 
  private:
    unsigned startRow, startCol, // Where to restart if willy dies.
	     jumpstate=0, // In jump? 
	                // 7 and 6 mean a jump is about to start.
			// 5 through 4 mean the worm is going up.
			// 3 through 1 mean the worm is going down.
			// 0 means the worm is not jumping.
	     jumpbase=0, // During a jump, this is the row the worm was in
	               // at the start of the jump. It is used to properly
		       // handle jumps with inadequate headroom.
	     lives; // Number of extra lives in reserve.
    int residual=0; // This is the number of points that have been earned
                  // (or lost if negative) since the last extra life
		  // was earned.
    Command dir=STOP; // Current direction the worm is moving.
    unsigned long score=0; // Score

    void gainLife(); // Call this when the residual gets high enough
                     // to add a new worm.

    /* Take care of the processing for any elements we are touching
     * just before (checkin) or just after (checkout) we move.
     * This involves calling the appropriate callbacks in the game.
     * Input:
     *   stepOff: indicates whether we stepped off anything.
     *   oldX: indicates which column we stepped off, if relevant.
     */
    void checkin(Game *game);
    void checkout(Game *game, bool stepOff, unsigned oldX);

    /* Utility function to try to act on a command.
     * If it is possible to complete the command, we do.
     * Return: indicates whether a backup command should be tried
     *   true: we were successful so no backup command is needed.
     *   false: we failed, so try to do something else.
     */
    bool ifLegal(Game *game, Command command);

  public:
    // Set up a new worm.
    Worm(const chtype name, unsigned row, unsigned col) :
      GameAgent(name, row, col), startRow(row), startCol(col), lives(4) {}
      
    // Clean up, but nothing special to do.
    ~Worm() {}

    // Do whatever processing there is to do on every turn.
    virtual bool clock(Game *game);

    // Process our death, which involves reducing our extra lives,
    // restaring the bonus counter and moving the worm to the start
    // locaton.
    virtual void die(Game *game);

    // This is not terribly useful, but if we had multiple worms it
    // would allow them to climb on each other.
    virtual bool isClimbable() { return true; }

    // Adjust our score. If we would go negative, we set the score to 0 and
    // the worm dies.
    virtual void addScore(Game *game, int points);

    // Report back a string with the current score and number of lives.
    // The game displays this on the status line.
    virtual std::string getStatus();
};
#endif
