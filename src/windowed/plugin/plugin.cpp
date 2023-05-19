// .. edited windowed mode from s0beit by immo.pr1nce ..

#include "plugin.h"

// used: directx9 types
#include <dinput.h>
#include <d3d9.h>

// used: game types
#include "misc.h"
using namespace game_api;

/*
 * you can make an API if you want, BUT ..
 * .. first add this function to the PLUGIN class, or make the rest shared
 * 
 * @startcode
 * 
 *  void ToggleWindowedMode() {
 *      set.request_change_windowed = true;
 *      set.request_change_video_mode = true;
 *      HandleWindowedMode( set.params );
 *  }
 * 
 * @endcode
 * 
 */

namespace {

struct Settings {
    // main settings

    bool windowed{ true };
    bool borderless{ true };

    bool request_change_windowed{ false };
    bool request_change_video_mode{ false };

    // temp settings

    bool force_update_active{ false };
    UINT fresh_rate{ 0 };

    D3DPRESENT_PARAMETERS *params{ nullptr };
} set;

int RwD3D9ChangeVideoMode( int mode ) {
    std::uintptr_t addr = 0x7F8640;

    __asm
    {
        push mode
        call addr
        add esp, 4
    }
}

void InitWindowedMode( D3DPRESENT_PARAMETERS *params ) {
    if ( params != PresentParams )
        return;

    set.fresh_rate = params->FullScreen_RefreshRateInHz;

    if ( set.windowed && im_mem::read<std::uint8_t>( 0x746225 ) != 0x90 ) {
        const auto x = ::GetSystemMetrics( SM_CXSCREEN ), y = ::GetSystemMetrics( SM_CYSCREEN );
        ::SetWindowLong( params->hDeviceWindow, GWL_STYLE, WS_POPUP );
        ::MoveWindow( params->hDeviceWindow, ( x / 2 ) - ( params->BackBufferWidth / 2 ), ( y / 2 ) - ( params->BackBufferHeight / 2 ), params->BackBufferWidth, params->BackBufferHeight, TRUE );

        params->Windowed                    = TRUE;
        params->FullScreen_RefreshRateInHz  = D3DPRESENT_RATE_DEFAULT;
    }
}

void HandleWindowedMode( D3DPRESENT_PARAMETERS *params ) {
    // check if nopped
    if ( im_mem::read<std::uint8_t>( 0x746225 ) == 0x90 ) {
        set.request_change_video_mode = false;
        set.request_change_windowed = false;
        return;
    }

    // window mode toggle, flips set.windowed bit
    if ( set.request_change_windowed ) {
        set.request_change_windowed = false;
        set.windowed ^= true;
    }

    // force the desired video mode if needed
    if ( ( set.windowed != (bool)params->Windowed || set.windowed != (bool)PresentParams->Windowed ) && !set.force_update_active ) {
        // get the current video mode & the correct back buffer size
        auto video_mode = RwD3D9DisplayMode::GetMode();

        const auto current_w = video_mode->width;
        const auto current_h = video_mode->height;

        // set windowed or fullscreen
        if ( set.request_change_video_mode ) {
            set.request_change_video_mode = false;
            set.force_update_active = true;

            // update video mode
            if ( RwD3D9ChangeVideoMode( 0 ) != 1 ) {
                set.force_update_active = false;
                return;
            }

            if ( set.windowed ) {
                im_mem::write<int>( 0xC920CC, 1 ); // _?windowed
                RsGlobalType::Instance().ps->fullscreen = 0;

                PresentParams->Windowed                     = TRUE;
                PresentParams->FullScreen_RefreshRateInHz   = D3DPRESENT_RATE_DEFAULT;
                PresentParams->BackBufferWidth              = current_w;
                PresentParams->BackBufferHeight             = current_h;

                params->Windowed                            = TRUE;
                params->FullScreen_RefreshRateInHz          = D3DPRESENT_RATE_DEFAULT;
                params->BackBufferWidth                     = current_w;
                params->BackBufferHeight                    = current_h;

                if ( RwD3D9ChangeVideoMode( 0 ) != 1 ) {
                    set.force_update_active = false;
                    return;
                }

                // RwD3D9DisplayMode::SetMode( 0 );
            } else {
                im_mem::write<int>( 0xC920CC, 0 ); // _?windowed
                RsGlobalType::Instance().ps->fullscreen = 1;

                PresentParams->Windowed                     = FALSE;
                PresentParams->FullScreen_RefreshRateInHz   = set.fresh_rate;
                PresentParams->BackBufferWidth              = current_w;
                PresentParams->BackBufferHeight             = current_h;

                params->Windowed                            = FALSE;
                params->FullScreen_RefreshRateInHz          = set.fresh_rate;
                params->BackBufferWidth                     = current_w;
                params->BackBufferHeight                    = current_h;

                if ( RwD3D9ChangeVideoMode( RwD3D9DisplayMode::GetModeId() ) != 1 ) {
                    set.force_update_active = false;
                    return;
                }

                // RwD3D9DisplayMode::SetMode( RwD3D9DisplayMode::GetModeId() );
            }

            set.force_update_active = false;
        } else if ( set.windowed ) {
            set.force_update_active = true;

            im_mem::write<int>( 0xC920CC, 1 ); // _?windowed
            RsGlobalType::Instance().ps->fullscreen = 0;

            PresentParams->Windowed                     = TRUE;
            PresentParams->FullScreen_RefreshRateInHz   = D3DPRESENT_RATE_DEFAULT;
            PresentParams->BackBufferWidth              = current_w;
            PresentParams->BackBufferHeight             = current_h;

            params->Windowed                            = TRUE;
            params->FullScreen_RefreshRateInHz          = D3DPRESENT_RATE_DEFAULT;
            params->BackBufferWidth                     = current_w;
            params->BackBufferHeight                    = current_h;

            set.force_update_active = false;
        }
    }

    // window mode stuff
    if ( set.windowed && params->BackBufferWidth != 0 && params->BackBufferHeight != 0 ) {
        // title bar or not?
        if ( set.borderless ) {
            // center the window
            ::SetWindowPos( params->hDeviceWindow, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOSIZE );
        } else {
            // add caption bar, etc
            ::SetWindowLong( params->hDeviceWindow, GWL_STYLE, WS_POPUP | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU | WS_VISIBLE );

            // update caption bar, etc
            ::SetWindowPos( params->hDeviceWindow, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOMOVE );

            RECT rect;

            // the client area of the window must be the same size as the back buffer
            ::GetClientRect( params->hDeviceWindow, &rect );
            if ( rect.right != params->BackBufferWidth || rect.bottom != params->BackBufferHeight ) {
                const auto x = ::GetSystemMetrics( SM_CXSCREEN ), y = ::GetSystemMetrics( SM_CYSCREEN );
                ::SetWindowPos( params->hDeviceWindow, HWND_NOTOPMOST, 0, 0, params->BackBufferWidth + ( params->BackBufferWidth - rect.right ), params->BackBufferHeight + ( params->BackBufferHeight - rect.bottom ), SWP_FRAMECHANGED | SWP_SHOWWINDOW | SWP_NOMOVE );
                ::SetWindowPos( params->hDeviceWindow, HWND_NOTOPMOST, ( x / 2 ) - ( params->BackBufferWidth / 2 ), ( y / 2 ) - ( params->BackBufferHeight / 2 ), params->BackBufferWidth + ( params->BackBufferWidth - rect.right ), params->BackBufferHeight + ( params->BackBufferHeight - rect.bottom ), SWP_SHOWWINDOW );
            }
        }
    }
}

HRESULT CSTDCALL HOOK_Reset( IDirect3DDevice9 *device, D3DPRESENT_PARAMETERS *params ) {
    const auto result = Plugin::hooks_.reset.call<im_mem::calling_convention::stdcall, HRESULT>( device, params );

    // handle windowed mode
    if ( SUCCEEDED( result ) ) HandleWindowedMode( set.params = params );

    return result;
}

void ReplaceRwD3D9Device() {
    if ( ( *RwD3D9Device ) == nullptr )
        return;

    Plugin::hooks_.reset.install( ( *reinterpret_cast<std::uintptr_t **>( ( *RwD3D9Device ) ) )[16], &HOOK_Reset );
}

void __declspec( naked ) HOOK_CreateDevice() {
    static std::uintptr_t jmp_addr{ 0x7F6781 + 3/*indirect call*/ + 2/*logical compare*/ };
    static HRESULT result{ 0 };

    __asm pushad

    // first init to prevent a crash
    InitWindowedMode( PresentParams );

    __asm
    {
        popad
        call dword ptr[edx + 40h]   // indirect call create device
        mov result, eax             // save result
        pushad
    }

    // hook d3d9 device reset
    if ( SUCCEEDED( result ) ) ReplaceRwD3D9Device();

    __asm
    {
        popad
        test eax, eax   // logical compare
        jmp jmp_addr    // move on to flow
    }
}

} // namespace anonimus

Plugin::Plugin() {
    this->hooks_.device_create.install( 0x7F6781, &HOOK_CreateDevice );
}

Plugin::~Plugin() {
    this->hooks_.reset.disable();
    this->hooks_.device_create.disable();
}
