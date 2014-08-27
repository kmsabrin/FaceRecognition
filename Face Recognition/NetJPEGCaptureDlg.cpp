//-----------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------
//--------------						KAESER MD. SABRIN					---------------------------
//--------------						DEPT. OF CSE						---------------------------
//--------------						UNIVERSITY OF DHAKA					---------------------------
//--------------						2008								---------------------------
//-----------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------
//--------------						FACE RECOGNITION					---------------------------
//--------------						EIGEN FACE							---------------------------
//-----------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------
#include "stdafx.h"
#include "NetJPEGCapture.h"
#include "NetJPEGCaptureDlg.h"
#include <vfw.h>
#include <time.h>
#include <math.h>
#include "faces.cpp"
#include "histogram_equalization_tawhid.cpp"
#include "contrast_stretching_tawhid.cpp"

// My Variables ---------------------------------------------------------------------------------------
#define MIN( p, q ) ((p) < (q)) ? (p) : (q)
#define MAX( p, q ) ((p) > (q)) ? (p) : (q)
#define PI acos( -1.0 )

#define IMAGE_W		132
#define IMAGE_H		132
#define IMAGE_SIZE	IMAGE_W * IMAGE_H
#define MAX_PERSON	200

int eye					= 1;
int interval			= 0;
int SPic				= 0;
int recognition_timer	= 0;

// My NEW Variables -----------------------------------------------------------------------------------
double		eigen_vectors_new		[MAX_PERSON + 10][IMAGE_SIZE + 10];
double		feature_vectors_new		[MAX_PERSON + 10][IMAGE_SIZE + 10];
double		test_image_new			[IMAGE_SIZE + 10];
double		test_feature_vector_new	[IMAGE_SIZE + 10];
double		mean_image_new			[IMAGE_SIZE + 10];
char		person_names			[MAX_PERSON     ][20];
char		person_image			[MAX_PERSON];
char		matcher					[MAX_PERSON];
char		mch_flg					[MAX_PERSON];
char		matcher_cnt;
int			n_person;
int			n_eigen_vector;
int			image_size;	
int			tawhid = 0;
int			Th = 27500000;
int			Inc = 5000000;
int			MAX_Th = 70000000;
int			debug = 0;

// I/O and Interface Handling Codes--------------------------------------------------------------------
HWND				hWndC;
int					CAPERROR;
int					frameCount = 0;
CNetJPEGCaptureDlg	*mydlg;
CString				str1;

//-----------------------------------------------------------------------------------------------------
LRESULT PASCAL FrameCallbackProc( HWND hWndC, LPVIDEOHDR lpVHdr )
{
	frameCount++;
	CString ftext;
	ftext.Format( "%d %d",frameCount, interval );
	mydlg->m_Message.SetWindowText(ftext);	
	//-------------------------------------------------------------------------------------------------
	int	i, ret, length;
	int	imagewidth, imageheight;	
	CAPSTATUS	s;
	ret = capGetStatus( hWndC, &s, sizeof(CAPSTATUS));	
	length = lpVHdr->dwBufferLength;
	imagewidth = s.uiImageWidth;		
	imageheight = s.uiImageHeight;		
	//-------------------------------------------------------------------------------------------------
	FIBITMAP *dib;
	dib = FreeImage_Allocate(imagewidth, imageheight, 24);
	unsigned char *buf = FreeImage_GetBits(dib);
	for(i=0;i<imagewidth*imageheight*3;i++)
		buf[i] = (int)lpVHdr->lpData[i];
	//-------------------------------------------------------------------------------------------------
	mydlg->doFilter(dib, buf, imagewidth, imageheight);
	mydlg->setImage(dib);
	//-------------------------------------------------------------------------------------------------
	time_t t;
	time( &t );
	CString TiMe = ctime( &t );
	TiMe = TiMe.Left( TiMe.GetLength() - 1 );			
	mydlg->m_Date.SetWindowText( TiMe );
	//-------------------------------------------------------------------------------------------------
	return (LRESULT)TRUE;
}

//-----------------------------------------------------------------------------------------------------
LONG CALLBACK exceptionhandler(EXCEPTION_POINTERS *ExceptionInfo)
{
	if( CAPERROR == 0 ) 
	{
		capDriverDisconnect( hWndC );	
	}
	return (EXCEPTION_CONTINUE_SEARCH);
}

//-----------------------------------------------------------------------------------------------------
class CAboutDlg : public CDialog
{
public:
	CAboutDlg();		
	enum { IDD = IDM_ABOUTBOX };	
protected:
	virtual void DoDataExchange(CDataExchange* pDX);		
protected:
	DECLARE_MESSAGE_MAP()
};

//-----------------------------------------------------------------------------------------------------
CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD){}

//-----------------------------------------------------------------------------------------------------
void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

//-----------------------------------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

//-----------------------------------------------------------------------------------------------------
/*CNetJPEGCaptureDlg::CNetJPEGCaptureDlg(CWnd* pParent)
: CDialog(CNetJPEGCaptureDlg::IDD, pParent)
, m_Image(NULL)
, m_useFilter(0)
{}*/
CNetJPEGCaptureDlg::CNetJPEGCaptureDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNetJPEGCaptureDlg::IDD, pParent)
	, m_Image(NULL)
	, m_useFilter(0)
{
	//m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

//-----------------------------------------------------------------------------------------------------
void CNetJPEGCaptureDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_Message);	
	DDX_Control(pDX, IDC_EDIT2, m_Date);	
	DDX_Control(pDX, IDC_EDIT3, m_Set);
	DDX_Control(pDX, IDC_EDIT4, m_Get);	
}

//-----------------------------------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(CNetJPEGCaptureDlg, CDialog)

ON_WM_SYSCOMMAND()
ON_WM_PAINT()
ON_WM_QUERYDRAGICON()

ON_BN_CLICKED( IDC_BUTTON1, OnBnClickedButton1 )
ON_BN_CLICKED( IDC_BUTTON2, OnBnClickedButton2 )
ON_BN_CLICKED( IDC_BUTTON3, OnBnClickedButton3 )
ON_BN_CLICKED( IDC_BUTTON4, OnBnClickedButton4 )
ON_BN_CLICKED( IDC_BUTTON5, OnBnClickedButton5 )
ON_BN_CLICKED( IDC_BUTTON6, OnBnClickedButton6 )
ON_BN_CLICKED( IDC_BUTTON7, OnBnClickedButton7 )
ON_BN_CLICKED( IDC_BUTTON8, OnBnClickedButton8 )

ON_BN_CLICKED( IDC_RADIO1,  OnBnClickedRadio1 )
ON_BN_CLICKED( IDC_RADIO2,  OnBnClickedRadio2 )
ON_BN_CLICKED( IDC_RADIO3,  OnBnClickedRadio3 )

END_MESSAGE_MAP()

//-----------------------------------------------------------------------------------------------------
BOOL CNetJPEGCaptureDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	//-------------------------------------------------------------------------------------------------
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);
	//-------------------------------------------------------------------------------------------------
	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL) {
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty()) {
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}	
	//-------------------------------------------------------------------------------------------------
	hWndC = capCreateCaptureWindow(
		"My Capture Window",
		WS_CHILD | WS_VISIBLE,
		10, 10, 320, 240,
		m_hWnd,
		0);
	//-------------------------------------------------------------------------------------------------
	if( capDriverConnect( hWndC, 0 ) == TRUE )
	{
		CAPERROR = 0;
	}
	else
	{
		CAPERROR = 1;
	}
	//-------------------------------------------------------------------------------------------------
	if( CAPERROR == 0 )
	{
		capPreviewRate( hWndC, 10 );	
		capPreviewScale( hWndC, FALSE );
	}
	SetUnhandledExceptionFilter(exceptionhandler);
	mydlg=this;
	CheckRadioButton(IDC_RADIO1,IDC_RADIO3,IDC_RADIO1);
	FRsdk::init("C:\\FVSDK_1_9\\huiconfig.txt");	
	return TRUE;
}

//-----------------------------------------------------------------------------------------------------
void CNetJPEGCaptureDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDD_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);	
	}
}
//-----------------------------------------------------------------------------------------------------
void CNetJPEGCaptureDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this);		
		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);		
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;
	}
	else
	{
		CPaintDC pDC(mydlg);				
		
		if(m_Image != NULL) 
		{
			::StretchDIBits(pDC.m_hDC, 
				340,10,320,240,
				0, 0, 320,240,
				FreeImage_GetBits(m_Image),
				FreeImage_GetInfo(m_Image),
				DIB_RGB_COLORS,            
				SRCCOPY                    
				);
		}		
		
		// My Code For Showing Mini Pic Of Identified Person's Image ----------------------------------
		if ( SPic ) 
		{
			FIBITMAP *bitmap = FreeImage_Load(FIF_BMP, person_image, BMP_DEFAULT);
			bitmap = FreeImage_Rescale( bitmap, 60, 60, FILTER_BOX );
			::StretchDIBits( pDC.m_hDC, 35, 250, 100, 100, 0, 0, 100, 100,
				FreeImage_GetBits(bitmap), FreeImage_GetInfo(bitmap), DIB_RGB_COLORS, SRCCOPY );				
		}
		else 
		{
			FIBITMAP *bitmap = FreeImage_Load(FIF_BMP, "FreeImage\\blank.bmp", BMP_DEFAULT);
			bitmap = FreeImage_Rescale( bitmap, 60, 60, FILTER_BOX );
			::StretchDIBits( pDC.m_hDC, 35, 250, 100, 100, 0, 0, 100, 100,
				FreeImage_GetBits(bitmap), FreeImage_GetInfo(bitmap), DIB_RGB_COLORS, SRCCOPY );				
		}		
	}
}

//-----------------------------------------------------------------------------------------------------
HCURSOR CNetJPEGCaptureDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//-----------------------------------------------------------------------------------------------------
void CNetJPEGCaptureDlg::setImage(FIBITMAP * img)
{
	CSingleLock singleLock(&m_CriticalSection);
	singleLock.Lock();
	if(m_Image!=NULL)
	{
		FreeImage_Unload(m_Image);
	}
	m_Image = img;
	singleLock.Unlock();
	Invalidate(FALSE);
}

// Send A Custom Interface Message---------------------------------------------------------------------
void SndMsg( int port, CString msg ) // port 0:left, 1:right-------------------------------------------
{
	CEdit &message = port ? mydlg->m_Get : mydlg->m_Set;	
	message.SetWindowText(msg);
}

// Read mean_image_new, eigen_space_vectors, feature_vectors_new from database---------------------------------
void ReadData()
{
	int		i;
	int		j; 
	FILE	*fp1;

	if(!tawhid)
		face();
	
	// read mean_image_new from file ----------------------------------------------------------------------
	if ( ( fp1 = fopen( "temp\\mean_image.txt", "r" ) ) == NULL )
	{
		exit( - 1 );
	}
	fscanf( fp1, "%d", &image_size );
	for ( i = 0; i < image_size; ++i )
	{
		fscanf( fp1, "%lf", &mean_image_new[i] );
	}
	fclose( fp1 );		
	
	// read eigen vectors from file -------------------------------------------------------------------
	if ( ( fp1 = fopen( "temp\\eigen_vectors.txt", "r" ) ) == NULL )
	{
		exit( - 1 );	
	}
	fscanf( fp1, "%d", &n_eigen_vector );				
	for ( i = 0; i < n_eigen_vector; ++i )
	{
		for ( j = 0; j < image_size; ++j )
		{
			fscanf( fp1, "%lf", &eigen_vectors_new[i][j] );
		}
	}				
	fclose( fp1 );			
	
	// read person counter ( number of data person ) --------------------------------------------------
	if ( ( fp1 = fopen( "temp\\person_counter.txt", "r" ) ) == NULL )
	{
		exit( - 1 );	
	}
	fscanf( fp1, "%d", &n_person );
	fclose( fp1 );	
	
	// read the person name list ----------------------------------------------------------------------
	if ( ( fp1 = fopen( "temp\\person_names.txt", "r" ) ) == NULL )
	{
		exit( - 1 );		
	}
	for ( i = 0; i < n_person; ++i )
	{
		fscanf( fp1, "%s", person_names[i] );
	}
	fclose( fp1 );	
	
	// read feature vectors for each person -----------------------------------------------------------
	if ( ( fp1 = fopen( "temp\\feature_vectors.txt", "r" ) ) == NULL )
	{
		exit( - 1 );					
	}
	for ( i = 0; i < n_person; ++i )
	{
		for ( j = 0; j < n_eigen_vector; ++j )
		{
			fscanf( fp1, "%lf", &feature_vectors_new[i][j] );
		}
	}
	fclose( fp1 );
}

// Start Button----------------------------------------------------------------------------------------
void CNetJPEGCaptureDlg::OnBnClickedButton1()
{
	frameCount = 0;
	capSetCallbackOnFrame( hWndC, FrameCallbackProc );
	capPreview( hWndC, TRUE );
	capOverlay( hWndC, TRUE );
	m_Message.SetWindowText( "Start Preview..." );
	ReadData();		
}

// Stop Button-----------------------------------------------------------------------------------------
void CNetJPEGCaptureDlg::OnBnClickedButton2()
{
	capPreview( hWndC, FALSE );	
	capOverlay( hWndC, FALSE );
	capSetCallbackOnFrame( hWndC, NULL );
	m_Message.SetWindowText("S topped..." );
	SndMsg( 0, "" );
}

// Format Button---------------------------------------------------------------------------------------
void CNetJPEGCaptureDlg::OnBnClickedButton3()
{
	capDlgVideoFormat( hWndC );	
}

// Source Button---------------------------------------------------------------------------------------
void CNetJPEGCaptureDlg::OnBnClickedButton4()
{
	capDlgVideoSource( hWndC );
}

// Exit Button----------------------------------------------------------------------------------------- 
void CNetJPEGCaptureDlg::OnBnClickedButton5()
{
	exit( -1 );	
}

// Read Info Button (OK button ) ----------------------------------------------------------------------
void CNetJPEGCaptureDlg::OnBnClickedButton6()
{		
	mydlg->m_Get.GetWindowText( str1 );
	CString t = "Adaptation of " + str1 + " Going On !!" + 
		        "                     Please Look At The Camera!!";
	SndMsg( 0, t );	
	SndMsg( 1, "Thank You !!" );	
	m_useFilter			= 3;
	interval			= 0;
	recognition_timer	= 0;
}

// Yes Button------------------------------------------------------------------------------------------
void CNetJPEGCaptureDlg::OnBnClickedButton7()
{	
	m_useFilter = 0;
	SndMsg( 1, "" );	
	SndMsg( 0, "Begining Adaptation: Enter name below: " );		
}

// No Button-------------------------------------------------------------------------------------------
void CNetJPEGCaptureDlg::OnBnClickedButton8()
{	
	m_useFilter = 2;		
}

// Radio Button = None---------------------------------------------------------------------------------
void CNetJPEGCaptureDlg::OnBnClickedRadio1()
{
	interval			= 0;
	recognition_timer	= 0;
	
	m_useFilter			= 0;
	mydlg->m_Get.SetWindowText		( "" );
	mydlg->m_Set.SetWindowText		( "" );		
	mydlg->m_Date.SetWindowText		( "" );
	mydlg->m_Message.SetWindowText	( "" );
	
}

// Radio Button = Gray---------------------------------------------------------------------------------
void CNetJPEGCaptureDlg::OnBnClickedRadio2()
{
	m_useFilter = 1;
}

// Radio Button = Face---------------------------------------------------------------------------------
void CNetJPEGCaptureDlg::OnBnClickedRadio3()
{
	m_useFilter			= 2;
	interval			= 0;
	recognition_timer	= 0;	
}

// check whether Face Box goes outside Array size -----------------------------------------------------
int bounded( int s, int w, int h )
{
	if ( s >= 0 && s < w * h * 3 )
	{
		return 1;
	}

	return 0;
}

// face box rotation function -------------------------------------------------------------------------
int getNew( int a, int w, int h, int cx, int cy, double t )
{	
	int myX  = ( a % ( w * 3 ) ) / 3;
	int myY  = h - a / ( w * 3 );
	
	int newX = (double)( myX - cx ) * cos( t ) - (double)( myY - cy ) * sin( t );
	int newY = (double)( myX - cx ) * sin( t ) + (double)( myY - cy ) * cos( t );
	
	newX += cx;
	newY += cy;		

	return ( h - newY ) * w * 3 + newX * 3;
}

// Mark The Face Region On The Image-------------------------------------------------------------------
void FaceLocation( unsigned char * buffer, int width, int height, int x, int y, int w, int ey1, int ey2, int r )
{	
	int		R = 0;
	int		G = 255;
	int		B = 96;
	int		a;
	int		b;
	int		c;
	int		d;
	int		i;	
	int		e;
	int		l;
	int		k;
	int		s;	
	int		D = abs( ey1 - ey2 );
	double	rt = 0;	
	int		f = 0;

	if ( D > 2 )
	{
		f = 1;

		rt = (double)D / (double)r;

		if ( ey1 > ey2 ) rt = -rt;
	}				
	
	// Draw A Box Surrounding Face Region -------------------------------------------------------------
	k = 7;	// box Thickness
	e = 20;
	l = 0;  // box Style
	w += 2;
	while ( k-- )
	{		
		// a = top edge, b = bottom edge, c = left edge, d = right edge
		a = ( height - y - w - e - l ) * width * 3 + ( x - w - l ) * 3;	
		b = ( height - y + w + 0 + l ) * width * 3 + ( x - w - l ) * 3;		
		
		c = ( height - y - w - e - l ) * width * 3 + ( x - w - l ) * 3;	
		d = ( height - y - w - e - l ) * width * 3 + ( x + w + l ) * 3;	
		//-------------------------------------------------------------------------------------------------
		for ( i = 0; i < 2 * ( w + l ); ++i )
		{
			s = a;											
			if ( f ) a = getNew( a, width, height, x, y, rt );						
			if ( bounded( a + 0, width, height ) ) buffer[a + 0] = B; 
			if ( bounded( a + 1, width, height ) ) buffer[a + 1] = G; 
			if ( bounded( a + 2, width, height ) ) buffer[a + 2] = R; 
			a = s;
			a += 3;		
			//---------------------------------------------------------------------------------------------
			s = b;											
			if ( f ) b = getNew( b, width, height, x, y, rt );	
			if ( bounded( b + 0, width, height ) ) buffer[b + 0] = B; 
			if ( bounded( b + 1, width, height ) ) buffer[b + 1] = G; 
			if ( bounded( b + 2, width, height ) ) buffer[b + 2] = R; 
			b = s;
			b += 3;		
		}
		//-------------------------------------------------------------------------------------------------
		for ( i = 0; i < 2 * ( w + e / 2 + l ); ++i )
		{
			s = c;											
			if ( f ) c = getNew( c, width, height, x, y, rt );	
			if ( bounded( c + 0, width, height ) ) buffer[c + 0] = B; 
			if ( bounded( c + 1, width, height ) ) buffer[c + 1] = G; 
			if ( bounded( c + 2, width, height ) ) buffer[c + 2] = R; 
			c = s;
			c += width * 3;				
			//---------------------------------------------------------------------------------------------
			s = d;											
			if ( f ) d = getNew( d, width, height, x, y, rt );	
			if ( bounded( d + 0, width, height ) ) buffer[d + 0] = B; 
			if ( bounded( d + 1, width, height ) ) buffer[d + 1] = G; 
			if ( bounded( d + 2, width, height ) ) buffer[d + 2] = R; 
			d = s;
			d += width * 3;
		}

		l += 1;
	}		
}

// Mark The Eye Region On The Image--------------------------------------------------------------------
void EyeLocation( unsigned char * buffer, int width, int height, int ex, int ey, int ex2, int ey2 )
{
	int		R = 250;
	int		G = 250;
	int		B = 0;
	int		i;
	int		a1;
	int		a2;
	int		b1;
	int		b2;
	int		c1;
	int		c2;
	int		d1;
	int		d2;	
	int		k;	// eyeSize
	int		b;  // box Thickness
	int		l;
	int		j;
	int		boX = 1;
	
	if ( boX )
	{
		k = 8;	// eyeSize
		b = 3;  // box Thickness
		l = 0;
		
		while ( b-- )
		{	
			j = 2 * ( k + l );
			
			// Draw Box On Each Eye
			a1 = width * ( height - ey  - k - l ) * 3 + ( ex  - k - l ) * 3;	
			b1 = width * ( height - ey  - k - l ) * 3 + ( ex  - k - l ) * 3;
			
			c1 = width * ( height - ey2 - k - l ) * 3 + ( ex2 - k - l ) * 3;	
			d1 = width * ( height - ey2 - k - l ) * 3 + ( ex2 - k - l ) * 3;	
			
			a2 = width * ( height - ey  + k + l ) * 3 + ( ex  - k - l ) * 3;	
			b2 = width * ( height - ey  - k - l ) * 3 + ( ex  + k + l ) * 3;	
			
			c2 = width * ( height - ey2 + k + l ) * 3 + ( ex2 - k - l ) * 3;	
			d2 = width * ( height - ey2 - k - l ) * 3 + ( ex2 + k + l ) * 3;	
			
			for ( i = 0; i < j; ++i ) 
			{
				if ( bounded( a1 + 0, width, height ) ) buffer[a1 + 0] = B; 
				if ( bounded( a1 + 1, width, height ) ) buffer[a1 + 1] = G; 
				if ( bounded( a1 + 2, width, height ) ) buffer[a1 + 2] = R;
				a1 += 3;
				
				if ( bounded( b1 + 0, width, height ) ) buffer[b1 + 0] = B;   
				if ( bounded( b1 + 1, width, height ) ) buffer[b1 + 1] = G; 
				if ( bounded( b1 + 2, width, height ) ) buffer[b1 + 2] = R; 
				b1 += width * 3;		
				
				if ( bounded( c1 + 0, width, height ) ) buffer[c1 + 0] = B; 
				if ( bounded( c1 + 1, width, height ) ) buffer[c1 + 1] = G; 
				if ( bounded( c1 + 2, width, height ) ) buffer[c1 + 2] = R;
				c1 += 3;
				
				if ( bounded( d1 + 0, width, height ) ) buffer[d1 + 0] = B;   
				if ( bounded( d1 + 1, width, height ) ) buffer[d1 + 1] = G; 
				if ( bounded( d1 + 2, width, height ) ) buffer[d1 + 2] = R; 
				d1 += width * 3;
				
				if ( bounded( a2 + 0, width, height ) ) buffer[a2 + 0] = B; 
				if ( bounded( a2 + 1, width, height ) ) buffer[a2 + 1] = G; 
				if ( bounded( a2 + 2, width, height ) ) buffer[a2 + 2] = R;
				a2 += 3;
				
				if ( bounded( b1 + 0, width, height ) ) buffer[b2 + 0] = B;   
				if ( bounded( b1 + 1, width, height ) ) buffer[b2 + 1] = G; 
				if ( bounded( b1 + 2, width, height ) ) buffer[b2 + 2] = R; 
				b2 += width * 3;		
				
				if ( bounded( c1 + 0, width, height ) ) buffer[c2 + 0] = B; 
				if ( bounded( c1 + 1, width, height ) ) buffer[c2 + 1] = G; 
				if ( bounded( c1 + 2, width, height ) ) buffer[c2 + 2] = R;
				c2 += 3;
				
				if ( bounded( d2 + 0, width, height ) ) buffer[d2 + 0] = B;   
				if ( bounded( d2 + 1, width, height ) ) buffer[d2 + 1] = G; 
				if ( bounded( d2 + 2, width, height ) ) buffer[d2 + 2] = R; 
				d2 += width * 3;
			}
			l += 1;
		}
	}
	else 
	{		
		k = 10;	// eyeSize
		b = 4;  // box Thickness		
		l = -2;
		
		while ( b-- )
		{	
			j = 2 * k;
			
			// Draw Cross On Each Eye
			a1 = width * ( height - ey - l ) * 3 + ( ex  - k ) * 3;	
			b1 = width * ( height - ey - k ) * 3 + ( ex - l ) * 3;
			
			c1 = width * ( height - ey2 - l ) * 3 + ( ex2 - k ) * 3;	
			d1 = width * ( height - ey2 - k ) * 3 + ( ex2 - l ) * 3;	
			
			for ( i = 0; i < j; ++i ) 
			{
				if ( bounded( a1 + 0, width, height ) ) buffer[a1 + 0] = B; 
				if ( bounded( a1 + 1, width, height ) ) buffer[a1 + 1] = G; 
				if ( bounded( a1 + 2, width, height ) ) buffer[a1 + 2] = R;
				a1 += 3;
				
				if ( bounded( b1 + 0, width, height ) ) buffer[b1 + 0] = B;   
				if ( bounded( b1 + 1, width, height ) ) buffer[b1 + 1] = G; 
				if ( bounded( b1 + 2, width, height ) ) buffer[b1 + 2] = R; 
				b1 += width * 3;		
				
				if ( bounded( c1 + 0, width, height ) ) buffer[c1 + 0] = B; 
				if ( bounded( c1 + 1, width, height ) ) buffer[c1 + 1] = G; 
				if ( bounded( c1 + 2, width, height ) ) buffer[c1 + 2] = R;
				c1 += 3;
				
				if ( bounded( d1 + 0, width, height ) ) buffer[d1 + 0] = B;   
				if ( bounded( d1 + 1, width, height ) ) buffer[d1 + 1] = G; 
				if ( bounded( d1 + 2, width, height ) ) buffer[d1 + 2] = R; 
				d1 += width * 3;						
			}
			l += 1;
		}
	}
}
//-----------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------------
// The (So Called) Main Function:: Called Upon Every Image Capture-------------------------------------
void CNetJPEGCaptureDlg::doFilter( FIBITMAP * fimg, unsigned char * buffer, int width, int height )
{
	//-------------------------------------------------------------------------------------------------				
	FILE			*fp1;
	FILE			*fp2;	
	
	int				i;
	int				j;
	int				k;
	int				match_i;
	int				flag; 
	int				match_person = 0;
	int				x;
	int				y;
	int				w;
	int				ex;
	int				ey;
	int				ex2;
	int				ey2;	
	int				h;
	int				f;
	int				extra = 30;
	
	unsigned char	match_ch;
	unsigned char	temp_image[IMAGE_SIZE + 10];	
	
	double			MiN = 1111111111;					
	
	//-------------------------------------------------------------------------------------------------			
	interval = ( interval == 400 ) ? (0) : (++interval); // Just Keep It Small-------------------------
	
	// Save The Current Frame As 'myTmp.jpg'-----------------------------------------------------------
	FreeImage_Save		( FIF_JPEG, fimg, "temp\\myTmp.jpg", JPEG_QUALITYSUPERB );
	FRsdk::Image img	( new FRsdk::JpegImage( "temp\\myTmp.jpg" ) );
	
	// Call FRSDK Library To Detect Face Location On Image---------------------------------------------	
	FRsdk::Face::LocationSet faceLocations				= m_faceFinder.find( img );
	FRsdk::Face::LocationSet::const_iterator faceIter	= faceLocations.begin();
	
	//-------------------------------------------------------------------------------------------------			
	switch( m_useFilter )
	{
	// Draw A Grey Image-------------------------------------------------------------------------------
	case 1:
	{
		int c = width * height * 3 - 1;
		
		while ( c >= 0 ) 
		{
			int s = ( buffer[c - 1] + buffer[c - 2] + buffer[c - 3] ) / 3;
			
			if ( s < 128 )
			{
				s -= (int) ( (double) s / 5.0 );
			}
			else if ( s >= 128 )
			{
				s += MAX( 255, (int) ( (double) s / 5.0 ) );
			}

			buffer[c - 1] = buffer[c - 2] = buffer[c - 3] = s;
			
			c -= 3;
		}
	}
	break;
	// End Of Case 01 ---------------------------------------------------------------------------------
	
	// Recogntion Phase--------------------------------------------------------------------------------
	case 2:
	{		
		f = 0;
		
		// Loop For Every Face Deteceted On An Image(usually 1) ---------------------------------------			
		while( faceIter != faceLocations.end() ) 
		{
			f = 1; // A Face Has Been Found		
			
			//-----------------------------------------------------------------------------------------
			x = ( int ) ( *faceIter ).pos.x();
			y = ( int ) ( *faceIter ).pos.y();
			w = ( int ) ( *faceIter ).width;
			h = w;
			
			// Draw Face-------------------------------------------------------------------------------
			// Draw Later :: FaceLocation( buffer, width, height, x, y, w );
			
			// Draw Eye--------------------------------------------------------------------------------
			if ( eye ) 
			{
				FRsdk::Eyes::LocationSet eyesLocations = eyesFinder.find( img, *faceIter );				
				FRsdk::Eyes::LocationSet::const_iterator eyesIter = eyesLocations.begin();				
				while( eyesIter != eyesLocations.end() ) 
				{
					ex  = ( int ) ( *eyesIter ).left.x();
					ey  = ( int ) ( *eyesIter ).left.y();					
					ex2 = ( int ) ( *eyesIter ).right.x();
					ey2 = ( int ) ( *eyesIter ).right.y();
					// Draw Later :: EyeLocation( buffer, width, height, ex, ey, ex2, ey2 );					
					++eyesIter;
				}
			}	
			
			// My Heuristic to adjust face size based on distance from camera -------------------------
			// Needed Here			
			if ( x - ex <= 21 )
			{
				w -= 8;
			}
			else if ( x - ex  <= 25 )
			{
				w -= 1;
			}
			else if ( x - ex  <= 30 )
			{
				w -= 0;
			}
			else
			{
				w += 1;
			}
						
			// Process An Image Every 4? Iteration-----------------------------------------------------			
			if ( interval % 4 == 0 ) 
			{				
				int k_bottom  = MIN( height - 1, y + w + extra );				
				int k_top     = MAX( 0, y - w );
				int k_left    = MAX( 0, x - w );
				int k_right   = MIN( width - 1, x + w ); 				
				
				// Cut And Resize The Image------------------------------------------------------------
				FIBITMAP * fimg1;
				FIBITMAP * fimg2;
				fimg1	= FreeImage_Copy			( fimg, k_left, k_top, k_right, k_bottom );
				fimg1	= FreeImage_Rescale			( fimg1, IMAGE_W, IMAGE_H, FILTER_BICUBIC );				
				fimg2	= FreeImage_ConvertTo8Bits	( fimg1 );
				FreeImage_Save						( FIF_BMP, fimg2, "temp\\my_tmp.bmp" );
				
				//DLL_API BOOL DLL_CALLCONV FreeImage_SetPixelIndex( FIBITMAP *dib, unsigned x, unsigned y, BYTE *value );
				//-------------------------------------------------------------------------------------
				//-------------------------------------------------------------------------------------
				
				// Begin Search For Recognizing The Face ----------------------------------------------								
				//-------------------------------------------------------------------------------------
				
				// read the test image ----------------------------------------------------------------
				int max_pixel_value = -1;
				int min_pixel_value = 1000;
				
				if ( ( fp1 = fopen( "temp\\my_tmp.bmp", "rb" ) ) == NULL )
				{
					exit( - 1 );													
				}
				
				if ( ( fp2 = fopen( "temp\\my_tmp_new.bmp", "wb" ) ) == NULL )
				{
					exit( - 1 );													
				}
				
				for ( match_i = 0; match_i < 1078; ++match_i )
				{	
					fscanf ( fp1, "%c", &match_ch );
					fprintf( fp2, "%c", match_ch  );
				}

				// fseek( fp1, 1078, 0 );									
				
				for ( match_i = 0; ; ++match_i )
				{			
					fscanf( fp1, "%c", &match_ch );
					
					if ( feof( fp1 ) )
					{
						break;
					}

					temp_image[match_i] = match_ch;
					
					//---------------------------------------------------------------------------------
					if ( match_ch < min_pixel_value ) 
					{
						min_pixel_value = match_ch;
					}
					
					//---------------------------------------------------------------------------------
					if ( match_ch > max_pixel_value ) 
					{
						max_pixel_value = match_ch;
					}
				}		
				
				fclose( fp1 );				
				
				// contrast stretch test image --------------------------------------------------------
				for ( i = 0; i < match_i; ++i )
				{
					temp_image[i] = ( ( double ) ( temp_image[i]	- min_pixel_value ) 
								    / ( double ) ( max_pixel_value	- min_pixel_value ) ) * 255.0;
				}
				
				// histogram equalized test image -----------------------------------------------------
				int				F    [256 + 10];
				int				CuF  [256 + 10];
				int				Feq  [256 + 10];
				int				CuFeq[256 + 10];
				int				s;
				int				e;
				int				d;
				int				f;
				unsigned char	II[256 + 10];
								
				memset( F,     0, sizeof( F     ) );
				memset( CuF,   0, sizeof( CuF   ) );
				memset( Feq,   0, sizeof( Feq   ) );
				memset( CuFeq, 0, sizeof( CuFeq ) );

				for ( j = 0; j < IMAGE_SIZE; ++j )
				{							
					++F[temp_image[j]];
				}		
				
				CuF[0] = F[0];

				for ( j = 1; j < 256; ++j )
				{
					
					CuF[j] = CuF[j - 1] + F[j];		
				}
				
				s = CuF[256 - 1];
				
				f = s / 256;		
				
				for ( j = 0; j < 256; ++j )
				{
					Feq[j] = f;
				}
				
				e = s - f * 256;
				
				for ( j = 128 - ( e / 2 ); j < 128 + ( e / 2 ); ++j )
				{
					++Feq[j];
				}
				
				CuFeq[0] = Feq[0];
				
				for ( j = 1; j < 256; ++j )
				{
					CuFeq[j] = CuFeq[j - 1] + Feq[j];
				}
				
				for ( j = 0; j < 256; ++j )
				{
					int min = 11111111;

					int map;
					
					for ( k = 0; k < 256; ++k )
					{
						d = abs( CuF[j] - CuFeq[k] );
						
						if ( d < min )
						{
							min = d;
							
							map = k;
						}
					}
					
					II[j] = map;			
				}	
				
				for ( j = 0; j < IMAGE_SIZE; ++j )
				{					
					test_image_new[j] = II[temp_image[j]];
					fprintf( fp2, "%c", II[temp_image[j]] );
				}

				fclose ( fp2 );
				
				// center test image ------------------------------------------------------------------
				for ( i = 0; i < image_size; ++i ) 
				{
					test_image_new[i] -= mean_image_new[i];
				}		

				// project the test image into eigenSpace ---------------------------------------------
				for ( i = 0; i < n_eigen_vector; ++i ) // for each eigenvector find dot product
				{
					double dot_product = 0; // for each entry of eigenvector 

					for ( j = 0; j < image_size; ++j )
					{
						dot_product += test_image_new[j] * eigen_vectors_new[i][j];
					}

					test_feature_vector_new[i] = dot_product;			
				}
				
				// find the euclidian distance---------------------------------------------------------
				flag = 0;		
				int m_p = -1;
				//FILE *d = fopen( "d.txt", "a" );
				//-------------------------------------------------------------------------------------
				for ( i = 0; i < n_person; ++i )
				{
					double distance = 0;					
					for ( j = 0; j < n_eigen_vector; ++j )
					{
						distance += ( test_feature_vector_new[j] - feature_vectors_new[i][j] ) * 
									( test_feature_vector_new[j] - feature_vectors_new[i][j] );												
					}					
					// distance = sqrt( distance ); // find L2 norm					
					//---------------------------------------------------------------------------------
					if ( distance < MiN )
					{
						MiN = distance;
						m_p = i;					
					}
					//fprintf( d, "%lf with %d\n", distance, i );
				}				
												
				if ( MiN < Th )
				{
					match_person = m_p;
					flag = 1;
				}
				
				// DEBUG :: 
				// match_person = m_p;
				// flag = 1;
				//fclose( d );
				//-------------------------------------------------------------------------------------
				//-------------------------------------------------------------------------------------
				
				//-------------------------------------------------------------------------------------				
				if ( flag == 0 ) // Failed To Identify This Person-----------------------------------------									
				{ 
					/*
					// After 4 Consecutive Failure, It's A New Person----------------------------------
					if ( ++recognition_timer >= 4 ) 
					{
						char myS[1000];
						sprintf( myS, "I Dont Know You, Do You Want to Register? %lf %s", min, person_names[match_person] );	
						SndMsg( 0, myS );					
						SPic = 0;
						recognition_timer = 0;				
					}
					*/						
					char myS[1000];
					//myS[0] = 0;
					if(!debug)
						sprintf( myS, "I Dont Know You, Do You Want to Register?" );
					else
						sprintf( myS, "I Dont Know You, Do You Want to Register? distance=%lf with=%s x=%d y=%d w=%d lx=%d ly=%d rx=%d ry=%d h=%d", MiN, person_names[m_p], x, y, w, ex, ey, ex2, ey2, h );						
					SndMsg( 0, myS );					
					SPic = 0;						
					// matcher_cnt = 0;
				}
				else // The Person Has Been Identified. Greet Him!!!-----------------------------------										 
				{ 	
					/*
					if ( interval % 3 == 0 )
					{
						int mx = -1;

						for ( i = 0; i < n_person; ++i )
						{
							if ( mch_flg[i] > mx )
							{
								mx = mch_flg[i];

								match_person = i;
							}

							mch_flg[i] = 0;
						}

						CString greeting;
						greeting = "Hi ";					
						greeting += person_names[match_person];
						greeting += " !!! How Are You?\0";									
						
						char dummy[15];						
						sprintf( dummy, "  Min Distance: %.2lf %d %d %d h=%d x=%d y=%d lx=%d ly=%d rx=%d ry=%d", MiN, width, height, sizeof( buffer ), h, x, y, ex, ey, ex2, ey2 );
						greeting += dummy;
						
						SndMsg( 0, greeting );
						
						sprintf( person_image, "contrast_images\\temp%d.bmp", match_person );
						SPic = 1;
						
						recognition_timer = 0;					
					}
					else
					{
						if ( match_person >= 0 )
						{
							++mch_flg[match_person];
						}
					}
					*/
					
					CString greeting;
					greeting = "Hi ";					
					greeting += person_names[match_person];
					greeting += " !!! How Are You?\0";									
					
					char dummy[15];						
					sprintf( dummy, "  Min Distance: %.2lf %d %d %d h=%d x=%d y=%d lx=%d ly=%d rx=%d ry=%d", MiN, width, height, sizeof( buffer ), h, x, y, ex, ey, ex2, ey2 );
					if(debug)
						greeting += dummy;
					
					SndMsg( 0, greeting );
					
					sprintf( person_image, "contrast_images\\temp%d.bmp", match_person );
					SPic = 1;
				}					
			}
			//-----------------------------------------------------------------------------------------
			FaceLocation( buffer, width, height, x, y, w, ey, ey2, ex2 - ex );
			EyeLocation ( buffer, width, height, ex, ey, ex2, ey2 );
			++faceIter;
		}						
		
		// No Face Has Been Detected-------------------------------------------------------------------
		if ( f == 0 )
		{
			SndMsg( 0, "No Face Detected!!" );
			//recognition_timer = 0;			
			//matcher_cnt = 0;
			//interval = 0;
			SPic = 0;	
		}
	}
	break;
	// End Of Case 02 ---------------------------------------------------------------------------------
	
	// Adaptation Phase--------------------------------------------------------------------------------
	case 3:	
	{			
		while( faceIter != faceLocations.end() ) 
		{				
			//-----------------------------------------------------------------------------------------
			x = ( int ) ( *faceIter ).pos.x();
			y = ( int ) ( *faceIter ).pos.y();
			w = ( int ) ( *faceIter ).width;
			// Draw Face-------------------------------------------------------------------------------
			// Draw Later :: FaceLocation( buffer, width, height, x, y, w );
			// Draw Eye--------------------------------------------------------------------------------
			if ( eye ) 
			{
				FRsdk::Eyes::LocationSet eyesLocations = eyesFinder.find( img, *faceIter );				
				FRsdk::Eyes::LocationSet::const_iterator eyesIter = eyesLocations.begin();				
				while( eyesIter != eyesLocations.end() ) 
				{
					ex  = ( int ) ( *eyesIter ).left.x();
					ey  = ( int ) ( *eyesIter ).left.y();					
					ex2 = ( int ) ( *eyesIter ).right.x();
					ey2 = ( int ) ( *eyesIter ).right.y();
					// Draw Later :: EyeLocation( buffer, width, height, ex, ey, ex2, ey2 );					
					++eyesIter;
				}
			}
			// My Heuristic to adjust face size based on distance from camera -------------------------			
			if ( x - ex <= 21 )
			{
				w -= 8;
			}
			else if ( x - ex  <= 25 )
			{
				w -= 1;
			}
			else if ( x - ex  <= 30 )
			{
				w -= 0;
			}
			else
			{
				w += 1;
			}			
			//-----------------------------------------------------------------------------------------						
			char a[1000];
			
			// Read Person Counter ----------------------------------------------------------------
			if ( ( fp1 = fopen( "temp\\person_counter.txt", "r" ) ) == NULL ) exit( - 1 );
			fscanf( fp1, "%d", &k );
			fclose( fp1 );
			
			// Increament Person Counter ----------------------------------------------------------
			if ( ( fp1 = fopen( "temp\\person_counter.txt", "w" ) ) == NULL ) exit( - 1 );
			fprintf( fp1, "%d", k + 1 );
			fclose( fp1 );
			
			// Insert Person's name ----------------------------------------------------------------
			if ( ( fp1 = fopen( "temp\\person_names.txt", "a" ) ) == NULL ) exit( - 1 );				
			fprintf( fp1, "%s\n", str1 );
			fclose( fp1 );
			
			// Get Fact Boundary ------------------------------------------------------------------
			int k_bottom  = MIN( height - 1, y + w + extra );				
			int k_top     = MAX( 0, y - w );
			int k_left    = MAX( 0, x - w );
			int k_right   = MIN( width - 1, x + w );
			
			// Save Person's Training Image -------------------------------------------------------
			FIBITMAP * fimg1;
			FIBITMAP * fimg2;
			
			fimg1 = FreeImage_Copy			( fimg, k_left, k_top, k_right, k_bottom );
			fimg1 = FreeImage_Rescale		( fimg1, IMAGE_W, IMAGE_H, FILTER_BICUBIC );				
			fimg2 = FreeImage_ConvertTo8Bits( fimg1 );
			
			sprintf( a, "captured_images\\temp%d.bmp", k );				
			FreeImage_Save( FIF_BMP, fimg2, a );
			
			// (??) DLL_API BOOL DLL_CALLCONV FreeImage_SetPixelIndex(FIBITMAP *dib, unsigned x, unsigned y, BYTE *value);
			// Return Back To Recognition Phase ---------------------------------------------------
			m_useFilter			= 2; 
			interval			= 0;
			recognition_timer	= 0;			
			SndMsg( 1, "" );							
			// Iterate For Other Faces ----------------------------------------------------------------
			//-----------------------------------------------------------------------------------------
			FaceLocation( buffer, width, height, x, y, w, ey, ey2, ex2 - ex );
			EyeLocation ( buffer, width, height, ex, ey, ex2, ey2 );
			++faceIter;
		}			
/*====================tawhid===========================================================*/
		FILE *ta;
		unsigned char tata;
		char a[1000], b[1000];

		sprintf( a, "captured_images\\temp%d.bmp", n_person );				
		sprintf( b, "contrast_images\\temp%d.bmp", n_person );				

		get_contrast_stretched_images_tawhid ( a, b );

		sprintf( a, "contrast_images\\temp%d.bmp", n_person );				
		sprintf( b, "histogram_images\\temp%d.bmp", n_person );				

		get_histogram_equalized_images_tawhid( a, b );	

		if ( ( ta = fopen( b, "rb" ) ) == NULL ) exit( - 1 );
		fseek( ta, 1078, 0 );

		i = 0;				

		while ( 1 )
		{
			fscanf( ta, "%c", &tata );

			if ( feof( ta ) ) break;

			test_image_new[i++] = tata;						
		}


		fclose( ta );

		// center test image ------------------------------------------------------------------
		for ( i = 0; i < image_size; ++i ) 
		{
			test_image_new[i] -= mean_image_new[i];
		}		
		
		// project the test image into eigenSpace ---------------------------------------------
		for ( i = 0; i < n_eigen_vector; ++i ) // for each eigenvector find dot product
		{
			double dot_product = 0; // for each entry of eigenvector 

			for ( j = 0; j < image_size; ++j )
			{
				dot_product += test_image_new[j] * eigen_vectors_new[i][j];
			}

			test_feature_vector_new[i] = dot_product;			
		}

		// Write feature vectors for each person -----------------------------------------------------------
		if ( ( ta = fopen( "temp\\feature_vectors.txt", "a" ) ) == NULL )
		{
			exit( - 1 );					
		}
		for ( j = 0; j < n_eigen_vector; ++j )
		{
			fprintf( ta, "%lf\n", test_feature_vector_new[j] );
		}
		fclose( ta );
		tawhid = 1;		
/*==================================================================================*/
		ReadData();
/*		if(tawhid = 1)
		{
			Th += Inc;
			if(Th > MAX_Th)
			{
				Th = MAX_Th;
			}
		}
		*/
	}

	break;
	// End Of Case 03 ---------------------------------------------------------------------------------
	
	}
	// End Of Switch ----------------------------------------------------------------------------------	
}
// End Of doFilter-------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------
//--------------						KAESER MD. SABRIN					---------------------------
//--------------						DEPT. OF CSE						---------------------------
//--------------						UNIVERSITY OF DHAKA					---------------------------
//--------------						2008								---------------------------
//-----------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------
		
/*
FILE *fp = fopen( "trouble.txt", "a" );

fprintf( fp, "%d %d %d\n", a, x, y );

fclose ( fp );
*/