// .. edited windowed mode from s0beit by immo.pr1nce ..

#include "plugin/plugin.h"

#ifndef _TEXT
#   ifdef _UNICODE
#       define _TEXT( x ) L##x
#   else
#       define _TEXT( x ) x
#   endif
#endif

constexpr auto PLUGIN_NAME = _TEXT( "windowed.asi" );

std::unique_ptr<Plugin> plugin;

BOOL APIENTRY DllMain( HMODULE hModule, DWORD reason_for_call, LPVOID lpReserved ) {
    if ( reason_for_call == DLL_PROCESS_ATTACH ) {
        DisableThreadLibraryCalls( hModule );

        if ( ::GetModuleHandle( _TEXT( "gta_sa.exe" ) ) == nullptr ) {
            ::MessageBox( nullptr, _TEXT( "This cannot be injected to another process.\nOpen <gta_sa.exe> to inject." ), PLUGIN_NAME, MB_OK );
            return FALSE;
        }

        plugin = std::make_unique<Plugin>();
    }

    return TRUE;
}
