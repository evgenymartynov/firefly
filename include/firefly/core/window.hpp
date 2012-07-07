#ifndef FIREFLY_WINDOW_HPP
#define FIREFLY_WINDOW_HPP

#include <firefly/common.hpp>
#include <firefly/core/videomode.hpp>

////////////////////////////////////////////////////////////////////////

namespace ff {

// contains information relevant to app window

    struct WindowSettings
    {
        bool  fullscreen;
        int   refreshRate;
        bool  accelerated;
        int   auxBufferCount;
        bool  noResize;
        bool  stereoRender;

// constructor

        WindowSettings() {
            refreshRate = 0;
            auxBufferCount = 0;
            fullscreen = GL_FALSE;
            stereoRender = GL_FALSE;
            noResize = GL_FALSE;
            accelerated = GL_FALSE;
        }
    };


// extended information about the current OpenGL context

    struct GLContext
    {
        int   majorVersion;
        int   minorVersion;
        bool  fwdCompat;
        bool  debug;
        int   profile;

        GLContext() :
            majorVersion (-1),
            minorVersion (-1),
            fwdCompat    (GL_FALSE),
            debug        (GL_FALSE),
            profile      (-1)
            { }
    };


// actual window implementation

    class Window
    {
    public:
        Window();
        ~Window();

// accessors

        const WindowSettings & GetSettings() const { return m_ws; }
        const VideoMode & GetVideoMode() const { return m_vm; }
        const GLContext & GetGLContext() const { return m_glc; }

        bool   Fullscreen() const { return m_ws.fullscreen; }
        uint32 GetWidth()   const { return m_vm.width; }
        uint32 GetHeight()  const { return m_vm.height; }

        void   Resize(uint32 width, uint32 height)
            { m_vm.width = width; m_vm.height = height; }

// internal helper

        bool Create(WindowSettings & ws,
                    GLContext & glc,
                    VideoMode & vm);

// external helper

    private:
        void GetActiveSettings    (WindowSettings & ws,
                                   GLContext & glc,
                                   VideoMode & vm);
        void ConfigWindowHints    (WindowSettings & ws,
                                   GLContext & glc,
                                   VideoMode & vm);
        void ReportWindowSettings (WindowSettings & ws,
                                   GLContext & glc,
                                   VideoMode & vm);

// private members

        WindowSettings m_ws;
        VideoMode      m_vm;
        GLContext      m_glc;
        VideoMode      m_vmDesktop;
    };

} // exiting namespace ff

////////////////////////////////////////////////////////////////////////

#endif


