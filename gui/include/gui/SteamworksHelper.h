//
// Created by zaiyichen on 2023/12/7.
//

#ifndef CPP_BUT_STEAMWORKSHELPER_H
#define CPP_BUT_STEAMWORKSHELPER_H

#include <steam/steam_api.h>
#include <gui/stdafx.h>
#include <windows.h>
#include <direct.h>
#include <eh.h>
#include <functional>

//-----------------------------------------------------------------------------
// Purpose: Wrapper around SteamAPI_WriteMiniDump which can be used directly
// as a se translator
//-----------------------------------------------------------------------------
extern "C" void MiniDumpFunction( unsigned int nExceptionCode, EXCEPTION_POINTERS *pException )
{
    MessageBox( nullptr, "Spacewar is crashing now!", "Unhandled Exception", MB_OK );

    // You can build and set an arbitrary comment to embed in the minidump here,
    // maybe you want to put what level the user was playing, how many players on the server,
    // how much memory is free, etc...
    //TODO
    SteamAPI_SetMiniDumpComment( "Minidump comment: SteamworksExample.exe\n" );

    // The 0 here is a build ID, we don't set it
    SteamAPI_WriteMiniDump( nExceptionCode, pException, 0 );
}


//-----------------------------------------------------------------------------
// Purpose: Helper to display critical errors
//-----------------------------------------------------------------------------
int Alert( const char *lpCaption, const char *lpText )
{
    return ::MessageBox( NULL, lpText, lpCaption, MB_OK );
}

//-----------------------------------------------------------------------------
// Purpose: callback hook for debug text emitted from the Steam API
//-----------------------------------------------------------------------------
extern "C" void __cdecl SteamAPIDebugTextHook( int nSeverity, const char *pchDebugText )
{
    // if you're running in the debugger, only warnings (nSeverity >= 1) will be sent
    // if you add -debug_steamapi to the command-line, a lot of extra informational messages will also be sent
    ::OutputDebugString( pchDebugText );

    if ( nSeverity >= 1 )
    {
        // place to set a breakpoint for catching API errors
        int x = 3;
        (void)x;
    }
}


//-----------------------------------------------------------------------------
// Purpose: Extracts some feature from the command line
//-----------------------------------------------------------------------------
bool ParseCommandLine( const char *pchCmdLine, const char **ppchServerAddress, const char **ppchLobbyID )
{
    // Look for the +connect ipaddress:port parameter in the command line,
    // Steam will pass this when a user has used the Steam Server browser to find
    // a server for our game and is trying to join it.
    const char *pchConnectParam = "+connect ";
    const char *pchConnect = strstr( pchCmdLine, pchConnectParam );
    *ppchServerAddress = NULL;
    if ( pchConnect && strlen( pchCmdLine ) > (pchConnect - pchCmdLine) + strlen( pchConnectParam ) )
    {
        // Address should be right after the +connect
        *ppchServerAddress = pchCmdLine + ( pchConnect - pchCmdLine ) + strlen( pchConnectParam );
    }

    // look for +connect_lobby lobbyid paramter on the command line
    // Steam will pass this in if a user taken up an invite to a lobby
    const char *pchConnectLobbyParam = "+connect_lobby ";
    const char *pchConnectLobby = strstr( pchCmdLine, pchConnectLobbyParam );
    *ppchLobbyID = NULL;
    if ( pchConnectLobby && strlen( pchCmdLine ) > (pchConnectLobby - pchCmdLine) + strlen( pchConnectLobbyParam ) )
    {
        // lobby ID should be right after the +connect_lobby
        *ppchLobbyID = pchCmdLine + ( pchConnectLobby - pchCmdLine ) + strlen( pchConnectLobbyParam );
    }

    return *ppchServerAddress || *ppchLobbyID;

}



//-----------------------------------------------------------------------------
// Purpose: Real main entry point for the program
//-----------------------------------------------------------------------------
static int RealMain( std::function<void()> MainLoop )
{

    if ( SteamAPI_RestartAppIfNecessary( k_uAppIdInvalid ) )
    {
        // if Steam is not running or the game wasn't started through Steam, SteamAPI_RestartAppIfNecessary starts the
        // local Steam client and also launches this game again.

        // Once you get a public Steam AppID assigned for this game, you need to replace k_uAppIdInvalid with it and
        // removed steam_appid.txt from the game depot.

        return EXIT_FAILURE;
    }


    // Init Steam CEG
    if ( !Steamworks_InitCEGLibrary() )
    {
        OutputDebugString( "Steamworks_InitCEGLibrary() failed\n" );
        Alert( "Fatal Error", "Steam must be running to play this game (InitDrmLibrary() failed).\n" );
        return EXIT_FAILURE;
    }

    // Initialize SteamAPI, if this fails we bail out since we depend on Steam for lots of stuff.
    // You don't necessarily have to though if you write your code to check whether all the Steam
    // interfaces are NULL before using them and provide alternate paths when they are unavailable.
    //
    // This will also load the in-game steam overlay dll into your process.  That dll is normally
    // injected by steam when it launches games, but by calling this you cause it to always load,
    // even when not launched via steam.
    if ( !SteamAPI_Init() )
    {
        OutputDebugString( "SteamAPI_Init() failed\n" );
        Alert( "Fatal Error", "Steam must be running to play this game (SteamAPI_Init() failed).\n" );
        return EXIT_FAILURE;
    }

    // set our debug handler
    SteamClient()->SetWarningMessageHook( &SteamAPIDebugTextHook );

    // Ensure that the user has logged into Steam. This will always return true if the game is launched
    // from Steam, but if Steam is at the login prompt when you run your game from the debugger, it
    // will return false.
    if ( !SteamUser()->BLoggedOn() )
    {
        OutputDebugString( "Steam user is not logged in\n" );
        Alert( "Fatal Error", "Steam user must be logged in to play this game (SteamUser()->BLoggedOn() returned false).\n" );
        return EXIT_FAILURE;
    }

    // do a DRM self check
    Steamworks_SelfCheck();


    if ( !SteamInput()->Init( false ) )
    {
        OutputDebugString( "SteamInput()->Init failed.\n" );
        Alert( "Fatal Error", "SteamInput()->Init failed.\n" );
        return EXIT_FAILURE;
    }
    char rgchCWD[1024];
    if ( !_getcwd( rgchCWD, sizeof( rgchCWD ) ) )
    {
        strcpy( rgchCWD, "." );
    }

    char rgchFullPath[1024];

    _snprintf( rgchFullPath, sizeof( rgchFullPath ), "%s\\%s", rgchCWD, "steam_input_manifest.vdf" );

    SteamInput()->SetInputActionManifestFilePath( rgchFullPath );

    MainLoop();

    // Shutdown the SteamAPI
    SteamAPI_Shutdown();

    // Shutdown Steam CEG
    Steamworks_TermCEGLibrary();

    // exit
    return EXIT_SUCCESS;
}

//int MainWithDump(HINSTANCE hInstance, LPSTR lpCmdLine,int nCmdShow, std::function<void()> MainLoop)
//{
//    // All we do here is call the real main function after setting up our se translator
//    // this allows us to catch exceptions and report errors to Steam.
//    //
//    // Note that you must set your compiler flags correctly to enable structured exception
//    // handling in order for this particular setup method to work.
//
//    if ( IsDebuggerPresent() )
//    {
//        // We don't want to mask exceptions (or report them to Steam!) when debugging.
//        // If you would like to step through the exception handler, attach a debugger
//        // after running the game outside of the debugger.
//        return RealMain( lpCmdLine, hInstance, nCmdShow , MainLoop);
//    }
//
//    _set_se_translator( MiniDumpFunction );
//    try  // this try block allows the SE translator to work
//    {
//        return RealMain( lpCmdLine, hInstance, nCmdShow , MainLoop);
//    }
//    catch( ... )
//    {
//        return -1;
//    }
//}


#endif //CPP_BUT_STEAMWORKSHELPER_H
