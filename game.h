#ifndef GAME_H
#define GAME_H

#include <list>
#include <memory>
#include <string>
#include <vector>

#include "display.h"

// Utility macro to help with mapping commands to strings that can
// be displayed.
#define INITCOMMAND(x) commandname[(x)]=#x;

class Game;

#include "display.h"

class GameAgent;
class GameDisplay;

/* Anything that can be displayed on the map. */
class GameElement
{ private:
    const chtype name; // The character to display.

  public:
    /* Create a game element that displays character name. */
    GameElement(const chtype name);

    /* Create a game element that displays a space. */
    GameElement();

    /* Clean up, but there is nothing to do. */
    virtual ~GameElement() {}

    /* Show the game element on the display at the location. */
    void draw(GameDisplay *disp, unsigned y, unsigned x)
    { disp->writeAt(y, x, name);
    }
};

/* A game element that can change the state of the game. */
class ActiveGameElement : public GameElement
{ public:
    /* Create a game element that displays character name. */
    ActiveGameElement(const chtype name) : GameElement(name) {}

    /* Clean up, but there is nothing to do. */
    virtual ~ActiveGameElement() {}

    /* Test if this element can be walked on top of. */
    virtual bool isSolid() { return false; }

    /* Test if this element will force someone who touched it to jump. */
    virtual bool isSpringy() { return false; }

    /* Test if this element can be climbed on. */
    virtual bool isClimbable() { return false; }

    /* Callback if an agent steps off this element.
     * Input:
     *   game: a reference to the game
     *   agent: the agent who stepped off the element.
     */
    virtual void stepOff(Game *game, GameAgent *agent) {}

    /* Callback if an agent touches this element.
     * Input:
     *   game: a reference to the game
     *   agent: the agent who touched the element.
     */
    virtual void touch(Game *game, GameAgent *agent) {}

    /* Callback if an agent jumps over this element.
     * Input:
     *   game: a reference to the game
     *   agent: the agent who jumped over the element.
     */
    virtual void jumpOver(Game *game, GameAgent *agent) {}
};

/* Elements that can be walked on. */
class SolidGameElement : public ActiveGameElement
{ public:
    /* Create a game element that displays character name. */
    SolidGameElement(const chtype name) : ActiveGameElement(name) {}

    // Clean up but there is nothing to do.
    virtual ~SolidGameElement() {}

    // Override the ActiveGameElement to make this kind solid.
    virtual bool isSolid() { return true; }
};

/* Elements that can be climbed on. */
class Ladder : public ActiveGameElement
{ public:
    /* Create a game element that displays character name. */
    Ladder(const chtype name) : ActiveGameElement(name) {}

    // Clean up but there is nothing to do.
    virtual ~Ladder() {}

    // Override the ActiveGameElement to make this kind climbable.
    virtual bool isClimbable() { return true; }
};

/* Elements that can be jumped on. */
class Spring : public ActiveGameElement
{ public:
    /* Create a game element that displays character name. */
    Spring(const chtype name) : ActiveGameElement(name) {}

    // Clean up but there is nothing to do.
    virtual ~Spring() {}

    // Override the ActiveGameElement to make this kind springy.
    virtual bool isSpringy() { return true; }
};

/* Class for elements that can move around. */
class GameAgent : public ActiveGameElement
{ protected:
    unsigned x, y; // Current location of the element.
    bool alive; // Indicates whether the agent is alive.

  public:
    // Constructor that will start the agent at 0,0 and alive.
    GameAgent(const chtype name) : ActiveGameElement(name), alive(true) {}

    // Constructor that will start the agent at a specific location and
    // alive.
    GameAgent(const chtype name, unsigned y, unsigned x)
    : ActiveGameElement(name), x(x), y(y), alive(true) {}

    // Clean up, but nothing to do.
    virtual ~GameAgent() {}
    
    /* Main function that is called on each turn.
     * The agent can do whatever it wants in this turn.
     * Return: Indicates whether the agent moved to a new location.
     */
    virtual bool clock(Game *game) { return false; }

    /* Called when the agent is killed. 
     * This version does nothing: the agent cannot be killed.
     */
    virtual void die(Game *game) {}

    /* Called when the game has decided to change the agent's score.
     * This version has no score so it does nothing.
     * Input:
     *   game: a reference to the game.
     *   points: the amount of adjustment (positive or negative).
     */
    virtual void addScore(Game *game, int points) {}

    /* Get a string that can be displayed as part of the status line. */
    virtual std::string getStatus() { return alive?"Alive":"Dead"; }

    /* Indicate whether the agent is alive. */
    bool isAlive() { return alive; }

    /* Return the current location of the agent. */
    unsigned getRow() { return y; }
    unsigned getCol() { return x; }

    /* Draw the agent at its current location. */
    void draw(GameDisplay *disp) { GameElement::draw(disp, y, x); }
};

class GameLevel
{
private:
    unsigned rows, columns, worm_row=0, worm_column=0, ball_row=0, ball_column=0;
    std::unique_ptr<GameElement*[]> elements;

public:
    GameLevel(unsigned rows, unsigned columns, char *level, GameElement **catalog):
        rows(rows), columns(columns)
    {   unsigned len=rows*columns;
        elements.reset(new GameElement*[len]);
        bool have_worm=false, have_ball=false;
    
        for(unsigned i=0; i<len; i++)
        {   unsigned char c=level[i];
        
            if(!have_worm&&(c==128||c==129))
            {   have_worm=true;
                getIndex(i, worm_row, worm_column);
            }
                
            if(!have_ball&&c==128+126)
            {   have_ball=true;
                getIndex(i, ball_row, ball_column);
            }
            
            elements[i]=catalog[c];
        }
    }
    
    inline void setWorm(unsigned row, unsigned column)
    {
        worm_row=row;
        worm_column=column;
    }
    
    inline GameElement **getLevel()
    {   return elements.get();
    }
    
    inline unsigned getIndex(unsigned row, unsigned column)
    {   return row+column*rows;
    }
    
    inline void getIndex(unsigned index, unsigned &row, unsigned &column)
    {
        column=index/rows;
        row=index-column*rows;
    }
    
    inline unsigned getRows() { return rows; }
    inline unsigned getColumns() { return columns; }
    inline unsigned getWormRow() { return worm_row; }
    inline unsigned getWormColumn() { return worm_column; }
    inline unsigned getBallRow() { return ball_row; }
    inline unsigned getBallColumn() { return ball_column; }
};
    
class Worm;

/* Class for the whole game.
 * TODO: Move the level into its own class, so that we can support
 *       multiple levels.
 */
class Game
{ private:
    /* The names of the commands, so that we can display them on the
     * user interface if we want to.
     */
    std::string commandname[NUMCOMMANDS];

    unsigned rows, columns, // The size of the map.
             startRow, startCol, // The starting location of Willy
             bonus, // The timer that counts down the bonus.
	                // Timer units does not match what is on the display.
             current_level=0; // The level that is currently active.

    /* level is an array of lists of game elements, one list for each
     * location in the map. The list stores all of the game elements that
     * are currently at that location. Whatever one is at the end of
     * the list is displayed.
     * TODO: Move this into a separate class, along with the logic for
     * loading a new level.
     */
    std::unique_ptr<std::list<GameElement *>[]> level;
    std::vector<GameLevel> levels;

    /* A mapping used to help load the level from disk. The bytes in the
     * file indicate what to display where. This array maps from byte to
     * element.
     */
    GameElement *catalog[256];

    GameDisplay *display; // The user interface.

    std::list<GameAgent *> agents; // A list of all agents so they can be
                                   // notified when it is their turn.

    Command command; // The command that is in effect for this turn.
    Worm *willy; // A reference for the main character.

    // Check if row and column are inside the map.
    // Note that if row or column is negative, it will become
    // huge when cast to an unsigned, and so will be correctly
    // flagged as invalid.
    bool valid(int row, int col)
    { return (unsigned)row<rows && (unsigned)col<columns;
    }

    // Helper utility to set up the commandnames array.
    void init()
    {   INITCOMMAND(STOP);
        INITCOMMAND(LEFT);
        INITCOMMAND(RIGHT);
        INITCOMMAND(UP);
        INITCOMMAND(DOWN);
        INITCOMMAND(JUMP);
        INITCOMMAND(DIE);
        INITCOMMAND(QUIT);
    }
    
    void switchLevel(int new_level);
    
    inline void switchLevel()
    {   switchLevel(current_level+1);
    }
    
  public:
    /*******************************************
     * These functions are called by main() to *
     * set up and manage the game.             *
     *******************************************/

    // Make a new game based on the given file.
    Game(const char *level, GameDisplay *display);

    // Clean up the game before releasing it. 
    // This one has lots to do!
    ~Game();

    bool clock(); // Function that handles everything that happens
                  // on every turn, like giving all the agents their turn.
                  
    // Start the game. This function does not return until the game ends.
    void start();

    // This function constructs and displays a status line, containing
    // the current bonus amount and Willy's status line.
    // The status line is displayed on the line below the map.
    // TODO: The status line is not diplayed if the map is higher than
    // the display. We should reserve a line for the status.
    void showStatus();

    /*******************************************
     * These functions are called by agents to *
     * interact with the game.                 *
     *******************************************/

    // Get the command from the user for this turn.
    Command getCommand() { return command; }

    // Override the current command. This can be useful for a number
    // of reasons.
    void setCommand(Command c) { display->setCommand(command=c); }

    // Find out if there is anything solid at the specified location.
    // It doesn't count if the only thing solid at this location is
    // the agent itself.
    bool hasSolid(GameAgent *agent, int row, int col);

    // Find out if there is anything springy at the specified location.
    // It doesn't count if the only thing springy at this location is
    // the agent itself.
    bool hasSpringy(GameAgent *agent, int row, int col);

    // Find out if there is anything climbable at the specified location.
    // It doesn't count if the only thing climbable as this location is
    // the agent itself.
    bool hasClimbable(GameAgent *agent, int row, int col);

    // Callback that the agent uses to report it stepped off the specified
    // column.
    void stepOff(GameAgent *agent, int col);

    // Callback that the agent uses to report it touched the specified
    // location.
    void touch(GameAgent *agent);

    // Callback that the agent uses to report it is jumping over the
    // specified row. The agent has to figure out what rows constitute
    // being jumped over, and call this for each jumped-over row.
    // The column will be taken from the agent's getCol().
    void jumpOver(GameAgent *agent, int y);

    // Used to indicate that the bonus counter needs to be reset.
    void reset();

    /*******************************************
     * These functions are generally useful.   *
     *******************************************/

    // Get the name of a command.
    std::string commandName(Command i) { return commandname[i]; }

    // Get the name of the current command.
    std::string commandName() { return commandname[command]; }

    // Get the current value of the bonus, which is different from the 
    // value of the bonus counter.
    // This can be used to get the number of bonus points to pass to 
    // willy->addScore() when the level is completed.
    // It is also used to construct the status line.
    int getBonus();

    /* Replace a GameElement at a given location.
     * Input:
     *   row, col: location
     *   e: GameElement to replace
     *   name: name of replacement element
     */
    void replace(unsigned row, unsigned col, GameElement *e, chtype c);
};

/* Elements that gives points. */
class Present : public ActiveGameElement
{ public:
    /* Create a game element that displays character name. */
    Present(const chtype name) : ActiveGameElement(name) {}

    // Clean up but there is nothing to do.
    virtual ~Present() {}

    // If touched, the present is removed, and the worm gets 100 points.
	int point = 100;
	virtual void touch(Game *game, GameAgent *agent) {
		agent->addScore(game, point);
		point=0;
		game->replace(agent->getRow(), agent->getCol(), agent, 1);
	}
};
#endif
