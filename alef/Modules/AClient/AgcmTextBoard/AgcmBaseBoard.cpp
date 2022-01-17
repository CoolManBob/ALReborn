#include "AgcmBaseBoard.h"

///Init///////////////////////////////////////////
RwCamera*           AgcmBaseBoard::ms_pCamera     = NULL;
AgcmTextBoardMng*   AgcmBaseBoard::ms_pMng        = NULL;
Image2D             AgcmBaseBoard::ms_Image2D;
//////////////////////////////////////////////////

AgcmBaseBoard::AgcmBaseBoard( eBoardType eType )
: m_Type( eType )
{
	m_Height = m_Recipz = 0;
	m_pClump            = NULL;
	m_bDraw             = FALSE;

	SetOffset( 0 , 0 );
	SetScreenPos( 0 , 0 , 0 );
	SetAlpha( 0 );

}

AgcmBaseBoard::~AgcmBaseBoard(void)
{
}

void AgcmBaseBoard::SetScreenPos( FLOAT x , FLOAT y , FLOAT z )
{
	RwV3d Tmp;
	Tmp.x  = x;   Tmp.y  = y;   Tmp.z   = z;
	SetScreenPos( Tmp );
}
