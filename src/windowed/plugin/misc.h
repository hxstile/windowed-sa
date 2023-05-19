// .. edited windowed mode from s0beit by immo.pr1nce ..

#pragma once

namespace game_api {

struct RwD3D9DisplayMode {
    unsigned int width;
    unsigned int height;
private:
    char __pad1[12];

public:

    // returns current display mode id
    static int &GetModeId() {
        return *reinterpret_cast<int *>( 0xC97C18 );
    }

    // returns current display mode
    static RwD3D9DisplayMode *GetMode() {
        return &RwD3D9DisplayMode::GetModes()[RwD3D9DisplayMode::GetModeId()];
    }

    // returns all modes array
    static RwD3D9DisplayMode *GetModes() {
        return *reinterpret_cast<RwD3D9DisplayMode **>( 0xC97C48 );
    }

    // changes current mode to selected
    static void SetMode( int mode ) {
        im_mem::callfunc::call<im_mem::calling_convention::cdeclcall, void>( 0x745C70, mode );
    }
};

struct RsGlobalType {
private:
    char __pad1[20];
public:
    struct {
    private:
        char __pad1[8];
    public:
        int                 fullscreen;
    private:
        char __pad2[8];
    public:
        IDirect3DDevice9   *diD3DDevice;
    private:
        char __pad3[16];
    } *ps;
private:
    char __pad2[36];

public:

    // returns the global reference to this class
    static RsGlobalType &Instance() {
        return *reinterpret_cast<RsGlobalType *>( 0xC17040 );
    }
};

inline auto PresentParams   = reinterpret_cast<D3DPRESENT_PARAMETERS *>( 0xC9C040 );
inline auto RwD3D9Device    = reinterpret_cast<IDirect3DDevice9 **>( 0xC97C28 );

} // namespace game_api
