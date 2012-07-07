#ifndef FIREFLY_VIDEOMODE_HPP
#define FIREFLY_VIDEOMODE_HPP

#include <firefly/opengl.hpp>
#include <firefly/common.hpp>

////////////////////////////////////////////////////////////////////////

namespace ff {

// forward definitions

    struct VideoMode;
    typedef vector<VideoMode> VideoModeList;

// contains videomode information regarding display

    struct VideoMode
    {
        // resolution
        uint32 width;
        uint32 height;

        // color depth
        uint8  red_bits;
        uint8  green_bits;
        uint8  blue_bits;
        uint8  alpha_bits;

        // depth / stencil buffers
        uint8  depth_bits;
        uint8  stencil_bits;

        // videomode options
        bool   vsync;
        uint8  FSAA;

// constructor

        VideoMode() :
            width(640),
            height(480),
            red_bits(8),
            green_bits(8),
            blue_bits(8),
            alpha_bits(8),
            depth_bits(16),
            stencil_bits(8),
            vsync(GL_FALSE),
            FSAA(0)
            { }

// destructor

        ~VideoMode() { }

// returns the color bit depth

        uint8 bpp() const
        {
            return (red_bits + green_bits + blue_bits + alpha_bits);
        }

// returns a human readable string of the video mode

        string str() const
        {
            stringstream buffer;
            buffer << width << "x" << height << "x" << (int)bpp();
            buffer << " VSync(" << (vsync ? "ON" : "OFF") << ") FSAA(";
            buffer << (int)FSAA << ") Depth " << (int)depth_bits;
            buffer << "-bit Stencil " << (int)stencil_bits << "-bit";
            return (buffer.str());
        }

// returns the current desktop video mode

        static const VideoMode GetDesktopMode()
        {
            GLFWvidmode mode;
            glfwGetDesktopMode(&mode);

            VideoMode vm;
            vm.width       = mode.Width;
            vm.height      = mode.Height;
            vm.red_bits    = mode.RedBits;
            vm.green_bits  = mode.GreenBits;
            vm.blue_bits   = mode.BlueBits;

            return vm;
        }

// enumerates all valid video modes for the system

        static const VideoModeList & GetSupportedModes()
        {
            static VideoModeList vml;
            if ( vml.empty() )
            {
                GLFWvidmode modes[256];
                int count =  glfwGetVideoModes(modes, 256);
                vml.reserve(count);

                for (int i = 0; i < count; ++i)
                {
                    VideoMode vm;
                    vm.width            = modes[i].Width;
                    vm.height           = modes[i].Height;
                    vm.red_bits         = modes[i].RedBits;
                    vm.green_bits       = modes[i].GreenBits;
                    vm.blue_bits        = modes[i].BlueBits;
                    vml.push_back(vm);
                }
            }
            return vml;
        }
    };

} // exiting namespace ff

////////////////////////////////////////////////////////////////////////

#endif


