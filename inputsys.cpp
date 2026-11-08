/******************************************************************************
 * inputsys.cpp: GameInput によるキーボード・マウス入力 */

#include "inputsys.h"

#include <array>
#include <cstdio>
#include <map>

#include <wrl/client.h>

#pragma comment( lib, "gameinput.lib" )

//-------- keybdmouse.cpp のコールバック
void mouseClick( int button , int state, int x, int y );
void mouseDrag( int x, int y );
void mouseMotion( int x, int y );
void charKeyDown( unsigned char key, int x, int y );
void charKeyUp( unsigned char key, int x, int y );
void funcKeyDown( int key, int x, int y );
void funcKeyUp( int key, int x, int y );

namespace {

using Microsoft::WRL::ComPtr;

constexpr uint32_t kMaxKeys = 16; // 同時押しとして読むキーの上限

constexpr std::array<GameInputMouseButtons, 3> kButtons = {
    GameInputMouseLeftButton, GameInputMouseMiddleButton, GameInputMouseRightButton,
};

ComPtr<IGameInput> gameInput;
HWND window = nullptr;

// 押されているキー（仮想キー → 押した時に決めた文字。文字でないキーは 0）
std::map<uint8_t, unsigned char> heldKeys;

GameInputMouseButtons heldButtons = GameInputMouseNone; // ウィンドウ内で押したボタン
int64_t lastMouseX = 0, lastMouseY = 0;
bool hasMouseReading = false;

struct ClientPos { int x, y; bool inside; };

ClientPos cursorPos()
{
    POINT p = {};
    GetCursorPos( &p );
    ScreenToClient( window, &p );
    RECT rc = {};
    GetClientRect( window, &rc );
    return { p.x, p.y, PtInRect( &rc, p ) != FALSE };
}

bool isModifier( uint8_t vk )
{
    switch( vk ){
      case VK_SHIFT: case VK_LSHIFT: case VK_RSHIFT:
      case VK_CONTROL: case VK_LCONTROL: case VK_RCONTROL:
      case VK_MENU: case VK_LMENU: case VK_RMENU:
      case VK_LWIN: case VK_RWIN: case VK_CAPITAL:
        return true;
      default:
        return false;
    }
}

/*-------- GLUT と同じく、ASCII の範囲のキーは文字として扱う
 * 英字は Shift に関係なく小文字、Esc・Enter・Delete などは制御文字 */
unsigned char toChar( const GameInputKeyState &key )
{
    switch( key.virtualKey ){
      case VK_ESCAPE: return 27;
      case VK_RETURN: return 13;
      case VK_BACK:   return 8;
      case VK_TAB:    return 9;
      case VK_DELETE: return 127;
      default: break;
    }
    if( key.virtualKey >= 'A' && key.virtualKey <= 'Z' ) return static_cast<unsigned char>( key.virtualKey - 'A' + 'a' );
    if( key.codePoint >= 0x20 && key.codePoint < 0x7F ) return static_cast<unsigned char>( key.codePoint );
    return 0;
}

void keyDown( uint8_t vk, unsigned char c, const ClientPos &pos )
{
    if( c != 0 ) charKeyDown( c, pos.x, pos.y );
    else         funcKeyDown( vk, pos.x, pos.y );
}

void keyUp( uint8_t vk, unsigned char c, const ClientPos &pos )
{
    if( c != 0 ) charKeyUp( c, pos.x, pos.y );
    else         funcKeyUp( vk, pos.x, pos.y );
}

// フォーカスを失った時など、押したままのキー・ボタンを全て離したことにする
void releaseAll( const ClientPos &pos )
{
    for( const auto &[vk, c] : heldKeys ){
        if( !isModifier( vk ) ) keyUp( vk, c, pos );
    }
    heldKeys.clear();

    for( GameInputMouseButtons button : kButtons ){
        if( heldButtons & button ) mouseClick( button, MOUSE_UP, pos.x, pos.y );
    }
    heldButtons = GameInputMouseNone;
}

void pollKeyboard( const ClientPos &pos )
{
    ComPtr<IGameInputReading> reading;
    if( FAILED( gameInput->GetCurrentReading( GameInputKindKeyboard, nullptr, &reading ) ) ) return;

    std::array<GameInputKeyState, kMaxKeys> keys = {};
    const uint32_t count = reading->GetKeyState( kMaxKeys, keys.data() );

    std::map<uint8_t, unsigned char> nowHeld;
    for( uint32_t i = 0; i < count; ++i ){
        const uint8_t vk = keys[i].virtualKey;
        const auto held = heldKeys.find( vk );
        // 押している間に文字が変わっても、離す時は押した時の文字で通知する
        nowHeld[vk] = ( held != heldKeys.end() ) ? held->second : toChar( keys[i] );
    }

    for( const auto &[vk, c] : heldKeys ){
        if( nowHeld.count( vk ) == 0 && !isModifier( vk ) ) keyUp( vk, c, pos );
    }
    for( const auto &[vk, c] : nowHeld ){
        if( heldKeys.count( vk ) == 0 && !isModifier( vk ) ) keyDown( vk, c, pos );
    }
    heldKeys = std::move( nowHeld );
}

void pollMouse( const ClientPos &pos )
{
    ComPtr<IGameInputReading> reading;
    if( FAILED( gameInput->GetCurrentReading( GameInputKindMouse, nullptr, &reading ) ) ) return;

    GameInputMouseState state = {};
    if( !reading->GetMouseState( &state ) ) return;

    // 移動: GameInput の位置は相対移動の累積なので、変化した時だけ通知する
    const bool moved = hasMouseReading && ( state.positionX != lastMouseX || state.positionY != lastMouseY );
    lastMouseX = state.positionX;
    lastMouseY = state.positionY;
    hasMouseReading = true;
    if( moved ){
        if( heldButtons != GameInputMouseNone ) mouseDrag( pos.x, pos.y );
        else if( pos.inside )                   mouseMotion( pos.x, pos.y );
    }

    // ボタン: ウィンドウ内で押したものだけを扱い、離す時はウィンドウ外でも通知する
    for( GameInputMouseButtons button : kButtons ){
        const bool down = ( state.buttons & button ) != 0;
        const bool held = ( heldButtons & button ) != 0;
        if( down && !held && pos.inside ){
            heldButtons |= button;
            mouseClick( button, MOUSE_DOWN, pos.x, pos.y );
        }
        else if( !down && held ){
            heldButtons &= ~button;
            mouseClick( button, MOUSE_UP, pos.x, pos.y );
        }
    }
}

}

namespace inputsys {

bool Init( HWND hwnd )
{
    window = hwnd;
    const HRESULT hr = GameInputCreate( &gameInput );
    if( FAILED( hr ) ){
        std::fprintf( stderr, "GameInputCreate failed: 0x%08lx\n", static_cast<unsigned long>( hr ) );
        return false;
    }
    // 他のアプリが前面にある間の入力は受け取らない
    gameInput->SetFocusPolicy( GameInputDisableBackgroundInput );
    return true;
}

void Shutdown()
{
    gameInput.Reset();
}

void Poll()
{
    if( !gameInput ) return;

    const ClientPos pos = cursorPos();
    if( GetForegroundWindow() != window ){
        releaseAll( pos );
        return;
    }
    pollKeyboard( pos );
    pollMouse( pos );
}

bool IsKeyDown( int virtualKey )
{
    auto held = [&]( int vk ) { return heldKeys.count( static_cast<uint8_t>( vk ) ) != 0; };
    switch( virtualKey ){
      case VK_SHIFT:   return held( VK_SHIFT )   || held( VK_LSHIFT )   || held( VK_RSHIFT );
      case VK_CONTROL: return held( VK_CONTROL ) || held( VK_LCONTROL ) || held( VK_RCONTROL );
      case VK_MENU:    return held( VK_MENU )    || held( VK_LMENU )    || held( VK_RMENU );
      default:         return held( virtualKey );
    }
}

}
