#include <firefly/core/window.hpp>
#include <firefly/debug/log.hpp>

////////////////////////////////////////////////////////////////////////

namespace ff {

// constructor

    Window::Window()
    {
    }

// destructor

    Window::~Window()
    {
    }


// loads the application window

    bool Window::Create(WindowSettings & ws,
                        GLContext & glc,
                        VideoMode & vm)
    {
        // store the current desktop video  mode
        m_vmDesktop = VideoMode::GetDesktopMode();

        // store the desired window parameters
        m_vm = vm;
        m_ws = ws;
        m_glc = glc;

        // pass settings to GLFW for configuring the window
        ConfigWindowHints(m_ws, m_glc, m_vm);

        // Create the window and OpenGL context
        if (!glfwOpenWindow( m_vm.width,
                             m_vm.height,
                             m_vm.red_bits,
                             m_vm.green_bits,
                             m_vm.blue_bits,
                             m_vm.alpha_bits,
                             m_vm.depth_bits,
                             m_vm.stencil_bits,
                             m_ws.fullscreen ? GLFW_FULLSCREEN
                                             : GLFW_WINDOW
                ))
        {
            glfwCloseWindow();
            return false;
        }
        ReportWindowSettings(m_ws, m_glc, m_vm);
        return true;
    }


// return settings of active window

    void Window::GetActiveSettings(WindowSettings & ws,
                                   GLContext & glc,
                                   VideoMode & vm)
    {
        // window settings
        ws.refreshRate    = glfwGetWindowParam(GLFW_REFRESH_RATE);
        ws.auxBufferCount = glfwGetWindowParam(GLFW_AUX_BUFFERS);
        ws.stereoRender   = glfwGetWindowParam(GLFW_STEREO) != 0;
        ws.noResize    = glfwGetWindowParam(GLFW_WINDOW_NO_RESIZE) != 0;
        ws.accelerated = glfwGetWindowParam(GLFW_ACCELERATED) != 0;

        // video mode
        vm.red_bits       = glfwGetWindowParam(GLFW_RED_BITS);
        vm.green_bits     = glfwGetWindowParam(GLFW_GREEN_BITS);
        vm.blue_bits      = glfwGetWindowParam(GLFW_BLUE_BITS);
        vm.alpha_bits     = glfwGetWindowParam(GLFW_ALPHA_BITS);
        vm.FSAA           = glfwGetWindowParam(GLFW_FSAA_SAMPLES);
        vm.stencil_bits   = glfwGetWindowParam(GLFW_STENCIL_BITS);
        vm.depth_bits     = glfwGetWindowParam(GLFW_DEPTH_BITS);

        int w = 0, h = 0;
        glfwGetWindowSize(&w, &h);

        vm.width = w;
        vm.height = h;

        // GL context
        glc.fwdCompat
            = glfwGetWindowParam(GLFW_OPENGL_FORWARD_COMPAT) != 0;
        glc.debug
            = glfwGetWindowParam(GLFW_OPENGL_DEBUG_CONTEXT)  != 0;
        glc.profile = glfwGetWindowParam(GLFW_OPENGL_PROFILE);

        GL_DEBUG(glGetIntegerv(GL_MAJOR_VERSION, &glc.majorVersion));
        GL_DEBUG(glGetIntegerv(GL_MINOR_VERSION, &glc.minorVersion));
    }


// pass window settings to glfw

    void Window::ConfigWindowHints(WindowSettings & ws,
                                   GLContext & glc,
                                   VideoMode & vm)
    {
        // window settings
        if (ws.refreshRate > 0)
            glfwOpenWindowHint(GLFW_REFRESH_RATE, ws.refreshRate);
        if (ws.auxBufferCount > 0)
            glfwOpenWindowHint(GLFW_AUX_BUFFERS, ws.auxBufferCount);
        if (ws.stereoRender)
            glfwOpenWindowHint(GLFW_STEREO, ws.stereoRender);
        if (ws.noResize)
            glfwOpenWindowHint(GLFW_WINDOW_NO_RESIZE, ws.noResize);

        // video mode
        if (vm.FSAA > 0)
            glfwOpenWindowHint(GLFW_FSAA_SAMPLES, vm.FSAA);

        // GL context
        if (glc.debug)
        {
            glfwOpenWindowHint(GLFW_OPENGL_DEBUG_CONTEXT,
                               glc.debug);
        }
        if (glc.majorVersion > 0)
        {
            glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR,
                               glc.majorVersion);
        }
        if (glc.minorVersion >= 0)
        {
            glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR,
                               glc.minorVersion);
        }
        if (glc.profile >= 0)
        {
            glfwOpenWindowHint(GLFW_OPENGL_PROFILE, glc.profile);
            glfwOpenWindowHint(GLFW_OPENGL_FORWARD_COMPAT,
                               glc.fwdCompat);
        }
    }


// writes the window settings to the global log

    void Window::ReportWindowSettings(WindowSettings & ws,
                                      GLContext & glc,
                                      VideoMode & vm)
    {
        VideoMode      activeVM;
        WindowSettings activeWS;
        GLContext      activeGLC;
        GetActiveSettings(activeWS, activeGLC, activeVM);

        // GL profile
        string profile("default");
        if (activeGLC.profile == GLFW_OPENGL_CORE_PROFILE)
        {
            profile = "CORE";
        }
        else if (activeGLC.profile == GLFW_OPENGL_COMPAT_PROFILE)
        {
            profile = "COMPATIBILITY";
        }

        g_Log.write(LOG_CONFIG, "Using %s OpenGL Profile.",
                    profile.c_str());

        // GL context
        g_Log.write(LOG_CONFIG, "OpenGL Context (%i.%i) : Forward"
                    "Compatible (%s) Debug (%s)",
                    activeGLC.majorVersion, activeGLC.minorVersion,
                    activeGLC.fwdCompat ? "YES" : "NO",
                    activeGLC.debug ? "YES" : "NO");

        if (glc.majorVersion > 0 && glc.minorVersion >= 0 &&
            (activeGLC.majorVersion != glc.majorVersion ||
             activeGLC.minorVersion != glc.minorVersion))
        {
            g_Log.write(LOG_WARNING, "Requested GL version %i.%i "
                        "not availible!", glc.majorVersion,
                        glc.minorVersion);
        }

        // hardware acceleration
        if (activeWS.accelerated != GL_TRUE)
            g_Log.write(LOG_WARNING, "%s Using Microsoft OpenGL"
                        " Software Renderer! (No Hardware "
                        "Acceleration)");

        // fullscreen
        if (ws.fullscreen && !activeWS.fullscreen)
            g_Log.write(LOG_ERROR, "Unable to enter fullscreen mode!");

        // resolution and color depth
        if (activeVM.width  != vm.width  ||
            activeVM.height != vm.height ||
            activeVM.bpp()  != vm.bpp())
        {
            g_Log.write(LOG_WARNING, "NOT SUPPORTED - %s",
                        vm.str().c_str());
        }
        g_Log.write(LOG_CONFIG, "Video Mode: %s",
                    activeVM.str().c_str());

        // set the vsync
        glfwSwapInterval(vm.vsync ? 1 : 0);

        // position the window & save settings
        int midX = (m_vmDesktop.width  / 2) - (activeVM.width  / 2);
        int midY = (m_vmDesktop.height / 2) - (activeVM.height / 2);
        glfwSetWindowPos(midX, midY);

        // Store the results
        vm  = activeVM;
        glc = activeGLC;
        ws  = activeWS;
    }

} // exiting namespace ff

////////////////////////////////////////////////////////////////////////
