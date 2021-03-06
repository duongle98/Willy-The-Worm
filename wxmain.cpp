/*********************************************************************
 * Name:      	main.cpp
 * Purpose:   	Implements simple wxWidgets application with GUI
 * 				created using wxFormBuilder.
 * Author:    
 * Created:   
 * Copyright: 
 * License:   	wxWidgets license (www.wxwidgets.org)
 * 
 * Notes:		Note that all GUI creation code is implemented in
 * 				gui.cpp source file which is generated by wxFormBuilder.
 *********************************************************************/

#include "display.h"

// initialize the application
IMPLEMENT_APP(MainApp);

////////////////////////////////////////////////////////////////////////////////
// application class implementation 
////////////////////////////////////////////////////////////////////////////////

bool MainApp::OnInit()
{
	SetTopWindow( new GameDisplay( NULL ) );
	GetTopWindow()->Show();
	
	// true = enter the main loop
	return true;
}

////////////////////////////////////////////////////////////////////////////////
// main application frame implementation 
////////////////////////////////////////////////////////////////////////////////

GameDisplay::GameDisplay(wxWindow *parent) : MainFrameBase( parent )
{
    #include "willychr.h"
    std::vector<wxBitmap> fontBitmaps;
    
    for(int i=0; i<fontlen; i++)
    {
        wxBitmap bitmap(font[i]);
        this->font.push_back(bitmap);
        this->fontScaled.push_back(wxBitmap(bitmap.ConvertToImage().Scale(24,24,wxIMAGE_QUALITY_NORMAL)));
    }
    
    game=new Game("willy1.txt", this);
    gameTimer.Start(300);
    game->clock();
}

GameDisplay::~GameDisplay()
{
}

void GameDisplay::OnCloseFrame(wxCloseEvent& event)
{
	Destroy();
}

void GameDisplay::OnExitClick(wxCommandEvent& event)
{
	Destroy();
}

void GameDisplay::OnAbortGame(wxCommandEvent& event)
{
	newCommand=QUIT;
}

void GameDisplay::OnKeyDown(wxKeyEvent& event)
{
    newCommand=command[(unsigned char)event.GetKeyCode()];
}

void GameDisplay::onPaint( wxPaintEvent& event ) 
{   int scale=spritesize.GetValue();
    int maxY, maxX;

    m_panel1.GetSize(&maxY, &maxX);
    maxY/=scale;
    maxX/=scale;

    wxClientDC draw(m_panel1);
    draw.DrawBitmap(fontScaled[1],3*scale,5*scale);
    
    if(needrefresh)
    {   if(width-x0<(unsigned)maxX) maxX=width-x0;
        if(height-y0<(unsigned)maxY) maxY=height-y0;
        
        for(int i=0; i<maxX; i++)
            for(int j=0; j<maxY; j++)
                draw.DrawBitmap(fontScaled[bufferEntry(j+y0,i+x0)],i*scale,j*scale);
    } else
    {
        for(int a: bufferUpdates)
        {
            int i=a/width, j=a-i*width;
            if(i>=x0 && j>=y0 && i<x0+maxX && j<y0+maxY)
                draw.DrawBitmap(fontScaled[bufferEntry(j,i)],(i-x0)*scale,(j-y0)*scale);
        }
    }
    
    bufferUpdates.clear();
}

void GameDisplay::onSpriteSizeUpdate( wxScrollEvent& event )
{   int scale=spritesize.GetValue();

    for(int i=0; i<fontlen; i++)
    {
        this->fontScaled[i]=wxBitmap(font[i].ConvertToImage().Scale(scale,scale,wxIMAGE_QUALITY_NORMAL));
    }
    
    needrefesh=true;
}

void GameDisplay::onSpeedUpdate( wxScrollEvent& event ) 
{ 
}

/**
* @brief Write the character c at the row and column of the virtual map.
*        The character is stored in a buffer in case the map is scrolled.
* @param row The row of the location, 0 the top row.
* @param column The column of the location, 0 the left column.
* @param c The character to write.
*/
void GameDisplay::writeAt(unsigned row, unsigned column, const chtype c)
{
    bufferEntry(row,column)=c;
}

/**
* @brief Write the message on the status line at the specified 
*        location. 
* @param row Not used.
* @param column The offset in the status line to write at.
* @param s The message.
*/
void GameDisplay::writeAt(unsigned row, unsigned column, const std::string s)
{
    std::string current=m_statusBar.GetStatusText();
    
    while(current.length()<column)
        current=current+"                              "
    
    m_statusBar.SetStatusText(current.substr(0,column)+s+current.substr(column+s.length()));
}

/**
* @brief If necessary, scroll so that the specified location is displayed.
* @param row The row of the location in the virtual map that must display.
* @param column The column of the location in the virtual map that must display.
* @param border The size of the border on the screen. If the location
*               is further from the edge than this border, we do not
*               scroll.
* @param scroll The unit of scrolling. 1 will give a smooth scroll,
*               larger numbers will hold the map still longer and jump
*               instead.
*/
void GameDisplay::center(unsigned row, unsigned column,
        unsigned border=10, unsigned scroll=1)
{   int scale=spritesize.GetValue();
    int maxY, maxX, move=y0+border-row;
    bool needrefresh=false;

    m_panel1.GetSize(&maxY, &maxX);
    maxY/=scale;
    maxX/=scale;

    if(y0 && move>0)
      { if((int)y0>move)
          y0-=move+scroll-(move-1)%scroll-1;
        else
          y0=0;

        needrefresh=true;
      } else
        if(y0<height-maxY&&(move=row-y0-maxY+border)>0)
        { if((int)(y0+maxY)-(int)height>move)
            y0+=move+scroll-(move-1)%scroll-1;
          else
            y0=height-maxY;
        
          needrefresh=true;
        }

      if(x0 && (move=x0+border-column)>0)
      { if((int)x0>move)
          x0-=move+scroll-(move-1)%scroll-1;
        else
          x0=0;

        needrefresh=true;
      } else
        if(x0<width-maxX&&(move=column-x0-maxX+border)>0)
        { if((int)(x0+maxX)-(int)width>move)
            x0+=move+scroll-(move-1)%scroll-1;
          else
            x0=width-maxX;

          needrefresh=true;
        }

 
}
        
/**
* @brief Get the command that was most recently issued by the user.
* @return The current command.
*/
Command GameDisplay::getCommand()
{
    return newCommand;
}

/**
* @brief  Set the next command, as if the user had entered it.
* @param command The new current command.
*/
void GameDisplay::setCommand(Command command)
{
    newCommand=command;
}

// TODO: Rename this to something else, because Game calls the clock method which should do nothing. 
// TODO: Call Game's clock here.
void GameDisplay::onTick( wxTimerEvent& event )
{
    int s=-speed.GetValue();
    if(gameTimer.GetInterval()!=s)
        gameTimer.Start(s);
        
    game->clock();
}