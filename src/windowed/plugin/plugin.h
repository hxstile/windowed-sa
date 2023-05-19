// .. edited windowed mode from s0beit by immo.pr1nce ..

#pragma once

// used: namespace im_mem
#include <im_mem/detail/im_hook.hpp>

class Plugin {
public:
    Plugin();
    virtual ~Plugin();

    struct {
        im_mem::hook reset;
        im_mem::hook device_create;
    } static inline hooks_;
};
