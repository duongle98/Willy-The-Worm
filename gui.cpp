///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Nov  6 2017)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "gui.h"

///////////////////////////////////////////////////////////////////////////

MainFrameBase::MainFrameBase( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	m_menuBar = new wxMenuBar( 0 );
	m_menuFile = new wxMenu();
	wxMenuItem* menuFileAbort;
	menuFileAbort = new wxMenuItem( m_menuFile, wxID_ABORT, wxString( _("Quit Game in Progress") ) + wxT('\t') + wxT("ESC"), wxEmptyString, wxITEM_NORMAL );
	m_menuFile->Append( menuFileAbort );
	menuFileAbort->Enable( false );
	
	wxMenuItem* menuFileExit;
	menuFileExit = new wxMenuItem( m_menuFile, wxID_EXIT, wxString( _("E&xit") ) + wxT('\t') + wxT("Alt+X"), wxEmptyString, wxITEM_NORMAL );
	m_menuFile->Append( menuFileExit );
	
	m_menuBar->Append( m_menuFile, _("&File") ); 
	
	m_menuMovement = new wxMenu();
	wxMenuItem* m_menuUp;
	m_menuUp = new wxMenuItem( m_menuMovement, wxID_ANY, wxString( _("Up") ) + wxT('\t') + wxT("UP"), wxEmptyString, wxITEM_NORMAL );
	m_menuMovement->Append( m_menuUp );
	
	wxMenuItem* m_menuDown;
	m_menuDown = new wxMenuItem( m_menuMovement, wxID_ANY, wxString( _("Down") ) + wxT('\t') + wxT("DOWN"), wxEmptyString, wxITEM_NORMAL );
	m_menuMovement->Append( m_menuDown );
	
	wxMenuItem* m_menuLeft;
	m_menuLeft = new wxMenuItem( m_menuMovement, wxID_ANY, wxString( _("Left") ) + wxT('\t') + wxT("LEFT"), wxEmptyString, wxITEM_NORMAL );
	m_menuMovement->Append( m_menuLeft );
	
	wxMenuItem* m_menuRight;
	m_menuRight = new wxMenuItem( m_menuMovement, wxID_ANY, wxString( _("Right") ) + wxT('\t') + wxT("RIGHT"), wxEmptyString, wxITEM_NORMAL );
	m_menuMovement->Append( m_menuRight );
	
	wxMenuItem* m_menuJump;
	m_menuJump = new wxMenuItem( m_menuMovement, wxID_ANY, wxString( _("Jump") ) + wxT('\t') + wxT("SPACE"), wxEmptyString, wxITEM_NORMAL );
	m_menuMovement->Append( m_menuJump );
	
	wxMenuItem* m_menuStop;
	m_menuStop = new wxMenuItem( m_menuMovement, wxID_ANY, wxString( _("Stop") ) + wxT('\t') + wxT("ANY"), wxEmptyString, wxITEM_NORMAL );
	m_menuMovement->Append( m_menuStop );
	
	m_menuBar->Append( m_menuMovement, _("Movement") ); 
	
	this->SetMenuBar( m_menuBar );
	
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );
	
	m_panel1 = new wxPanel( this, wxID_BOARD, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	mainSizer->Add( m_panel1, 1, wxEXPAND|wxBOTTOM, 5 );
	
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxHORIZONTAL );
	
	spriteSizeLabel = new wxStaticText( this, wxID_ANY, _("Zoom"), wxDefaultPosition, wxDefaultSize, 0 );
	spriteSizeLabel->Wrap( -1 );
	bSizer2->Add( spriteSizeLabel, 0, wxALL, 5 );
	
	spritesize = new wxSlider( this, wxID_ANY, 24, 8, 48, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL );
	bSizer2->Add( spritesize, 0, wxALL, 5 );
	
	speedLabel = new wxStaticText( this, wxID_ANY, _("Speed"), wxDefaultPosition, wxDefaultSize, 0 );
	speedLabel->Wrap( -1 );
	bSizer2->Add( speedLabel, 0, wxALL, 5 );
	
	speed = new wxSlider( this, wxID_ANY, -300, -1000, -100, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL );
	bSizer2->Add( speed, 0, wxALL, 5 );
	
	
	mainSizer->Add( bSizer2, 0, wxEXPAND, 5 );
	
	
	this->SetSizer( mainSizer );
	this->Layout();
	m_statusBar = this->CreateStatusBar( 1, wxST_SIZEGRIP, wxID_ANY );
	gameTimer.SetOwner( this, wxID_ANY );
	
	this->Centre( wxBOTH );
	
	// Connect Events
	this->Connect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( MainFrameBase::OnCloseFrame ) );
	this->Connect( wxEVT_KEY_DOWN, wxKeyEventHandler( MainFrameBase::onKeyDown ) );
	m_menuBar->Connect( wxEVT_KEY_DOWN, wxKeyEventHandler( MainFrameBase::onKeyDown ), NULL, this );
	this->Connect( menuFileAbort->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameBase::onAbortGame ) );
	this->Connect( menuFileExit->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameBase::OnExitClick ) );
	this->Connect( m_menuUp->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameBase::onGoUp ) );
	this->Connect( m_menuDown->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameBase::onGoDown ) );
	this->Connect( m_menuLeft->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameBase::onGoLeft ) );
	this->Connect( m_menuRight->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameBase::onGoRight ) );
	this->Connect( m_menuJump->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameBase::onGoJump ) );
	this->Connect( m_menuStop->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameBase::onGoStop ) );
	m_panel1->Connect( wxEVT_KEY_DOWN, wxKeyEventHandler( MainFrameBase::onKeyDown ), NULL, this );
	m_panel1->Connect( wxEVT_PAINT, wxPaintEventHandler( MainFrameBase::onPaint ), NULL, this );
	m_panel1->Connect( wxEVT_SIZE, wxSizeEventHandler( MainFrameBase::onResize ), NULL, this );
	spriteSizeLabel->Connect( wxEVT_KEY_DOWN, wxKeyEventHandler( MainFrameBase::onKeyDown ), NULL, this );
	spritesize->Connect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( MainFrameBase::onSpriteSizeUpdate ), NULL, this );
	spritesize->Connect( wxEVT_KEY_DOWN, wxKeyEventHandler( MainFrameBase::onKeyDown ), NULL, this );
	speedLabel->Connect( wxEVT_KEY_DOWN, wxKeyEventHandler( MainFrameBase::onKeyDown ), NULL, this );
	speed->Connect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( MainFrameBase::onSpeedUpdate ), NULL, this );
	speed->Connect( wxEVT_KEY_DOWN, wxKeyEventHandler( MainFrameBase::onKeyDown ), NULL, this );
	m_statusBar->Connect( wxEVT_KEY_DOWN, wxKeyEventHandler( MainFrameBase::onKeyDown ), NULL, this );
	this->Connect( wxID_ANY, wxEVT_TIMER, wxTimerEventHandler( MainFrameBase::onTick ) );
}

MainFrameBase::~MainFrameBase()
{
	// Disconnect Events
	this->Disconnect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( MainFrameBase::OnCloseFrame ) );
	this->Disconnect( wxEVT_KEY_DOWN, wxKeyEventHandler( MainFrameBase::onKeyDown ) );
	m_menuBar->Disconnect( wxEVT_KEY_DOWN, wxKeyEventHandler( MainFrameBase::onKeyDown ), NULL, this );
	this->Disconnect( wxID_ABORT, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameBase::onAbortGame ) );
	this->Disconnect( wxID_EXIT, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameBase::OnExitClick ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameBase::onGoUp ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameBase::onGoDown ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameBase::onGoLeft ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameBase::onGoRight ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameBase::onGoJump ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrameBase::onGoStop ) );
	m_panel1->Disconnect( wxEVT_KEY_DOWN, wxKeyEventHandler( MainFrameBase::onKeyDown ), NULL, this );
	m_panel1->Disconnect( wxEVT_PAINT, wxPaintEventHandler( MainFrameBase::onPaint ), NULL, this );
	m_panel1->Disconnect( wxEVT_SIZE, wxSizeEventHandler( MainFrameBase::onResize ), NULL, this );
	spriteSizeLabel->Disconnect( wxEVT_KEY_DOWN, wxKeyEventHandler( MainFrameBase::onKeyDown ), NULL, this );
	spritesize->Disconnect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( MainFrameBase::onSpriteSizeUpdate ), NULL, this );
	spritesize->Disconnect( wxEVT_KEY_DOWN, wxKeyEventHandler( MainFrameBase::onKeyDown ), NULL, this );
	speedLabel->Disconnect( wxEVT_KEY_DOWN, wxKeyEventHandler( MainFrameBase::onKeyDown ), NULL, this );
	speed->Disconnect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( MainFrameBase::onSpeedUpdate ), NULL, this );
	speed->Disconnect( wxEVT_KEY_DOWN, wxKeyEventHandler( MainFrameBase::onKeyDown ), NULL, this );
	m_statusBar->Disconnect( wxEVT_KEY_DOWN, wxKeyEventHandler( MainFrameBase::onKeyDown ), NULL, this );
	this->Disconnect( wxID_ANY, wxEVT_TIMER, wxTimerEventHandler( MainFrameBase::onTick ) );
	
}
