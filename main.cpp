/******************************************************************************
 * main.cpp
 * ウィンドウ・OpenGL コンテキストは Win32/WGL、キーボード・マウスは GameInput で扱い、
 * 以前の GLUT のコールバックと同じ順で呼び出す */

#include <cstdio>
#include <cstdlib>

#include "glheaders.h"
#include "inputsys.h"
#include "memsys.h"

#include "balloongame.h"
#include "common.h"

//-------- callback functions
void display( void );
void update( void );
void reshape( int width, int height );

bool initWindow( char *title );

WindowDataT window;

int time;

namespace {

constexpr char kWindowClass[] = "MarionetteWindow";

HWND hwnd = nullptr;
HDC hdc = nullptr;
HGLRC glContext = nullptr;

// 動かすインゲーム。枠は memsys のプールヒープから切り出す
memsys::ObjectPool<BalloonGame> gamePool;
InGamePlay *game = nullptr;

UINT originalConsoleOutputCP = 0;

/*------------------------------------------------------------------- setupConsole
 * setupConsole - 文字列リテラルは UTF-8 なので、コンソールの出力も UTF-8 にする
 * コンソールは起動元と共有なので、終了時（[Q] の exit() を含む）に元へ戻す
 *--------*/
void setupConsole()
{
	originalConsoleOutputCP = GetConsoleOutputCP();
	if( originalConsoleOutputCP == 0 || originalConsoleOutputCP == CP_UTF8 ) return;
	SetConsoleOutputCP( CP_UTF8 );
	std::atexit( []{ SetConsoleOutputCP( originalConsoleOutputCP ); } );
}

/*------------------------------------------------------------------- windowProc
 * windowProc - ウィンドウのメッセージ（入力は GameInput で読むのでここでは扱わない）
 *--------*/
LRESULT CALLBACK windowProc( HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	switch( msg ){
	  case WM_SIZE:
		reshape( LOWORD( lParam ), HIWORD( lParam ) );
		return 0;
	  case WM_ERASEBKGND:
		return 1; // 背景は OpenGL で塗るので消去しない（ちらつき防止）
	  case WM_DESTROY:
		PostQuitMessage( 0 );
		return 0;
	  default:
		return DefWindowProcA( wnd, msg, wParam, lParam );
	}
}

/*------------------------------------------------------------------- setSwapInterval
 * setSwapInterval - WGL_EXT_swap_control があれば垂直同期を設定する
 *--------*/
void setSwapInterval( int interval )
{
	using SwapIntervalProc = BOOL (WINAPI *)( int );
	const auto proc = reinterpret_cast<SwapIntervalProc>( wglGetProcAddress( "wglSwapIntervalEXT" ) );
	if( proc != nullptr ) proc( interval );
}

/*------------------------------------------------------------------- shutdownWindow
 * shutdownWindow - OpenGL コンテキストとウィンドウを破棄する
 *--------*/
void shutdownWindow()
{
	if( glContext != nullptr ){
		wglMakeCurrent( nullptr, nullptr );
		wglDeleteContext( glContext );
		glContext = nullptr;
	}
	if( hwnd != nullptr ){
		if( hdc != nullptr ) ReleaseDC( hwnd, hdc );
		DestroyWindow( hwnd );
		hwnd = nullptr;
		hdc = nullptr;
	}
}

}

/*------------------------------------------------------------------- update
 * update - 毎フレームのシーン更新
 *--------*/
void update( void )
{
	time = static_cast<int>( GetTickCount64() );

    game->Update();

	return;
}

/*------------------------------------------------------------------- display
 * display - 描画
 *--------*/
void display( void )
{
	glEnable( GL_NORMALIZE ); //// 20161109追加

    //-------- viewport --------
	glViewport( 0, 0, window.width, window.height );

    //-------- clear buffers --------
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	//-------- projection transformation --------
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
	Projection();

	//-------- viewing transformation --------
    glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
    Viewing( *game );

    //-------- draw --------
	glEnable( GL_DEPTH_TEST ); // ---- begin:
    glEnable( GL_LIGHTING );
	glPushMatrix();
    DrawScene( *game );
	glPopMatrix();
    glDisable( GL_LIGHTING );
    glDisable( GL_DEPTH_TEST );

	//-------- swapbuffers --------
    SwapBuffers( hdc );

    return;
}
/*--------------------------------------------------------------------- reshape
 * reshape - ウィンドウサイズ変更
 *--------*/
void reshape( int width, int height )
{
	if( height <= 0 ) return; // 最小化中は縦横比を変えない

	window.width = width;
	window.height = height;
	window.aspect = (float)window.width/window.height;
}
/*---------------------------------------------------------------- initWindow
 * initWindow: initializes OpenGL window
 *--------*/
bool initWindow( char *winname )
{
	//////// ユーザカスタマイズ
	window.xo = 100;
	window.yo = 100;
	window.width = 800;
	window.height = 600;
	////////////////

	window.aspect = (float)window.width/window.height; // aspect ratio
	window.title = winname;

	// 高DPI環境でも拡大でぼやけないよう、座標をピクセル単位で扱う
	SetProcessDpiAwarenessContext( DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2 );

	const HINSTANCE instance = GetModuleHandleA( nullptr );
	WNDCLASSEXA wc = {};
	wc.cbSize = sizeof( wc );
	wc.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = windowProc;
	wc.hInstance = instance;
	wc.hCursor = LoadCursor( nullptr, IDC_ARROW );
	wc.lpszClassName = kWindowClass;
	if( RegisterClassExA( &wc ) == 0 ){
		std::fprintf( stderr, "RegisterClassEx failed: %lu\n", GetLastError() );
		return false;
	}

	//-------- open window（クライアント領域が width x height になるよう枠の分を足す）
	const DWORD style = WS_OVERLAPPEDWINDOW;
	RECT rect = { 0, 0, window.width, window.height };
	AdjustWindowRect( &rect, style, FALSE );
	hwnd = CreateWindowExA( 0, kWindowClass, window.title, style,
	                        window.xo, window.yo, rect.right - rect.left, rect.bottom - rect.top,
	                        nullptr, nullptr, instance, nullptr );
	if( hwnd == nullptr ){
		std::fprintf( stderr, "CreateWindowEx failed: %lu\n", GetLastError() );
		return false;
	}
	hdc = GetDC( hwnd );

    //-------- config buffers（GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH 相当）
	PIXELFORMATDESCRIPTOR pfd = {};
	pfd.nSize = sizeof( pfd );
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 24;
	pfd.iLayerType = PFD_MAIN_PLANE;
	const int format = ChoosePixelFormat( hdc, &pfd );
	if( format == 0 || !SetPixelFormat( hdc, format, &pfd ) ){
		std::fprintf( stderr, "SetPixelFormat failed: %lu\n", GetLastError() );
		return false;
	}

	glContext = wglCreateContext( hdc );
	if( glContext == nullptr || !wglMakeCurrent( hdc, glContext ) ){
		std::fprintf( stderr, "wglCreateContext failed: %lu\n", GetLastError() );
		return false;
	}

	// 垂直同期で約60Hzに揃える（シミュレーションはフレーム単位で進むため）
	setSwapInterval( 1 );

	ShowWindow( hwnd, SW_SHOW );
	SetForegroundWindow( hwnd );

	RECT client = {};
	GetClientRect( hwnd, &client );
	reshape( client.right, client.bottom );

    return true;
}
/*---------------------------------------------------------------------- main
 * main
 *--------*/
int main( int argc, char *argv[] )
{
	setupConsole();

	//////// ユーザカスタマイズ：起動時にまとめて確保するメモリ
	memsys::Config memConfig;
	memConfig.reserveSize   = 1 * memsys::GiB;  // 予約するアドレス空間の総量
	memConfig.frameSize     = 16 * memsys::MiB; // 毎フレーム空になる領域
	memConfig.poolSize      = 64 * memsys::MiB; // プール（プレイヤー・風船など）の枠を切り出す領域
	memConfig.initialCommit = 64 * memsys::MiB; // 常駐領域のうち起動時にコミットする量
	memConfig.commitStep    = 1 * memsys::MiB;  // プールヒープ・常駐領域が足りない時に追加する単位
	////////////////

	if( !memsys::Init( memConfig ) ) return 1;

    if( !initWindow( argv[0] ) ){
		shutdownWindow();
		return 1;
	}
	if( !inputsys::Init( hwnd ) ){
		shutdownWindow();
		return 1;
	}

	memsys::PrintStats();

    printf( "[H]:Help\n" );     // indicate help instruction

	//-------- インゲームを作って始める
	if( !gamePool.Init( memsys::PoolHeap(), 1 ) || ( game = gamePool.Create() ) == nullptr ){
		std::fprintf( stderr, "インゲームのメモリを確保できませんでした\n" );
		shutdownWindow();
		return 1;
	}
	if( !game->Start() ){
		gamePool.DestroyAll();
		shutdownWindow();
		return 1;
	}

	time = static_cast<int>( GetTickCount64() );

	//-------- main loop
	bool running = true;
	while( running ){
		MSG msg;
		while( PeekMessageA( &msg, nullptr, 0, 0, PM_REMOVE ) ){
			if( msg.message == WM_QUIT ) running = false;
			TranslateMessage( &msg );
			DispatchMessageA( &msg );
		}
		if( !running || game->IsFinished() ) break; // ルール上の終わりでも抜ける

		memsys::BeginFrame(); // 前のフレームの一時確保を捨てる
		inputsys::Poll(); // キーボード・マウスのコールバックはここから呼ばれる
		update();
		display();
	}

	// インゲームを終えて片付ける（風船などのデストラクタを、memsys を閉じる前に呼ぶ）
	game->Finish();
	gamePool.DestroyAll();
	game = nullptr;
	inputsys::Shutdown();
	shutdownWindow();

	memsys::PrintStats();
	memsys::Shutdown();

    return 0;
}
/******************************************************************************
 * end of source code */
