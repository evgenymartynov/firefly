#include <firefly/core/app.hpp>
#include <firefly/io/ini_file.hpp>
#include <firefly/debug/gl_debug.hpp>

// handle the main function
int main(int argc, char **arv)
{
    ff::App kate_beckinsale_rocks_my_world;
    assert(ff::App::get_singleton_ptr());

    if (!g_App.init())
    {
        glfwTerminate();
        g_Log.write(LOG_ERROR, "app::init > fatal error encountered!");
        return FF_EXIT_FAILURE;
    }
    else
    {
        g_App.main_loop(); // --> greatest video game ever... tribute
        g_App.shutdown();  // --x so long, and thanks for all the fish
    }
    return FF_EXIT_SUCCESS;
}

////////////////////////////////////////////////////////////////////////

namespace ff {

// constructor

    App::App()
    {
        m_numProcessors = 0;
        m_appTitle = "Firefly - OpenGL framework by John Cramb";
        m_bRunning = false;
        m_bActive = true;
        m_bAutoPause = false;
        m_frameTime = 0;
        m_gameTime = 0;
        m_runTime = 0;
        m_fpsAvg = 0;
        m_fpsMin = 99999;
        m_fpsMax = 0;
        m_msPerFrameAvg = 0;
        m_msPerFrameMin = 99999;
        m_msPerFrameMax = 0;
    }


// destructor

    App::~App()
    {
    }


// write to log via app

    void App::Log(const char * format, ...)
    {
        assert(format);
        static string strbuf;
        char buffer[256];
        va_list va;
        va_start(va, format);
        vsnprintf(buffer, 256, format, va);
        va_end(va);
        g_Log.write(m_runTime, strbuf = buffer);
    }


// update window title

    void App::SetWindowTitle(const char * title, ...)
    {
        char buffer[256];
        va_list va;
        va_start(va, title);
        vsnprintf(buffer, 256, title, va);
        va_end(va);
        glfwSetWindowTitle(buffer);
    }


// configure glfw for the app

    void App::configure_app()
    {
        glfwSetWindowSizeCallback(ffOnWindowResize);
        glfwSetWindowCloseCallback(ffOnWindowClose);
        glfwSetKeyCallback(ffOnKeyEvent);
        glfwSetCharCallback(ffOnText);
        glfwSetMouseButtonCallback(ffOnMouseButton);
        glfwSetMousePosCallback(ffOnMouseMove);
        glfwSetMouseWheelCallback(ffOnMouseWheel);

        glfwEnable(GLFW_STICKY_MOUSE_BUTTONS);
        glfwEnable(GLFW_STICKY_KEYS);
        glfwEnable(GLFW_MOUSE_CURSOR);
        glfwEnable(GLFW_SYSTEM_KEYS);
        glfwEnable(GLFW_KEY_REPEAT);
    }


// load app config settings from ini file

    bool App::load_config(const string & filePath,
                     WindowSettings & ws,
                     GLContext & glc,
                     VideoMode & vm)
    {
        ini_file config;
        if (!config.load(filePath))
            g_Log.write(LOG_ERROR, "app::load_config > could not find"
                        "file '%s'", filePath.c_str());

        config.select("APP");
        g_Log.set(config.get("Log", FF_LOG_FILE));
        m_bAutoPause = config.get<bool>("AutoPause", false);

        config.select("GRAPHICS");
        ws.fullscreen = config.get<bool>("Fullscreen", ws.fullscreen);
        ws.noResize = config.get<bool>("NoResize", ws.noResize);
        vm.width = config.get<int>("Width", vm.width);
        vm.height = config.get<int>("Height", vm.height);
        vm.red_bits = config.get<int>("RedBits", vm.red_bits);
        vm.green_bits = config.get<int>("GreenBits", vm.green_bits);
        vm.blue_bits = config.get<int>("BlueBits", vm.blue_bits);
        vm.alpha_bits = config.get<int>("AlphaBits", vm.alpha_bits);
        vm.depth_bits = config.get<int>("Depth", vm.depth_bits);
        vm.stencil_bits = config.get<int>("Stencil", vm.stencil_bits);
        vm.FSAA = config.get<int>("FSAA", vm.FSAA);
        vm.vsync = config.get<bool>("VSync", vm.vsync);

        config.select("OPENGL");
        glc.majorVersion = config.get<int>("Major", glc.majorVersion);
        glc.minorVersion = config.get<int>("Minor", glc.minorVersion);
        glc.debug = config.get<bool>("Debug", glc.debug);
        glc.fwdCompat = config.get<bool>("FwdCompat", glc.fwdCompat);

        int coreProfile = config.get<int>("CoreProfile", -1);
        if (coreProfile == 1) glc.profile = GLFW_OPENGL_CORE_PROFILE;
        if (coreProfile == 0) glc.profile = GLFW_OPENGL_COMPAT_PROFILE;

        return true;
    }


// app loop functions

    bool App::init()
    {
        assert(!m_bRunning);
        WindowSettings ws;
        VideoMode vm;
        GLContext glc;

        g_Log.write(LOG_NOTIME, " ");
        g_Log.write(LOG_EVENT, "Firefly - C/C++ OpenGL framework by "
                    "John Cramb (http://www.geekdownunder.net)");
        g_Log.write(LOG_NOTIME, " ");

        // load config file
        if (!load_config(FF_CONFIG_FILE, ws, glc, vm))
        {
            g_Log.write(LOG_ERROR, "app::init > failed to load config");
            return false;
        }

        // init glfw
        if (glfwInit() == GL_TRUE)
        {
            int x, y, z;
            glfwGetVersion(&x, &y, &z);
            g_Log.write(LOG_NOTIME, "Using GLFW Version %i.%i.%i",
                        x, y, z);
        }
        else
        {
            g_Log.write(LOG_ERROR, "app::init > failed to init GLFW!");
        }

        // get info about host computer
        m_numProcessors = glfwGetNumberOfProcessors();
        g_Log.write(LOG_NOTIME, "CPU: (detected %i logical cores)",
                    m_numProcessors);

        // create the app window
        if (m_window.Create(ws, glc, vm))
        {
            configure_app();
        }
        else
        {
            g_Log.write(LOG_ERROR, "app::init > can't create window");
            return false;
        }

        // fixes an issue with glew + CORE_PROFILE
        if (m_window.GetGLContext().profile == GLFW_OPENGL_CORE_PROFILE)
            glewExperimental = GL_TRUE;

        // init glew to set up OpenGL extensions
        if (glewInit() != GLEW_OK)
        {
            g_Log.write(LOG_ERROR, "app::init > failed to init GLEW!");
            return false;
        }

        // start sub-systems

        g_Log.write(LOG_NOTIME, " ");
        g_Log.write(LOG_EVENT, "Loading Game...");

        Load();

        g_Log.write(LOG_NOTIME, " ");
        return (m_bRunning = true);
    }


// the main game loop

    void App::main_loop()
    {
        assert(m_bRunning);
        Log("... main loop ...");
        delta_t frameTime = 0;
        m_timer.start();

        while (m_bRunning)
        {
            frameTime = GetFrameTime();
            frame_update(m_frameTime, m_gameTime);
            frame_render(m_frameTime, m_gameTime);
            update_timer();
        }
    }


// clean up the framework

    void App::shutdown()
    {
        Exit();

        // shutdown each subsystem
		m_timer.stop();

        glfwTerminate();
        g_Log.write(LOG_NOTIME, " ");

        // record some stats
        int mins = static_cast<int>(m_runTime * 0.0166666666666667);
        float secs = (float)m_runTime - static_cast<float>(mins * 60);
        g_Log.write(LOG_NOTIME, "Running Time: %i mins %.2f secs",
                    mins, secs);
        g_Log.write(LOG_NOTIME, "%.2f fps high, %.2f fps low. "
                    "(Avg: %.2f fps)", m_fpsMax, m_fpsMin, m_fpsAvg);
        g_Log.write(LOG_NOTIME, "%.5f ms/F high, %.5f ms/F low. "
                    "(Avg: %.5f ms/F)", m_msPerFrameMax,
                    m_msPerFrameMin, m_msPerFrameAvg);
        g_Log.write(LOG_NOTIME, " ");
    }


// perform one 'tick' of the game

    void App::frame_update(const delta_t dt, const delta_t elapsed)
    {
        bool active = (glfwGetWindowParam(GLFW_ACTIVE) != 0);
        if (m_bActive != active)
        {
            if (active) on_active();
            else on_inactive();
        }

        Update(dt, elapsed);
    }


// render the current frame

    void App::frame_render(const delta_t dt, const delta_t elapsed)
    {
        Render(dt, elapsed);
        glfwSwapBuffers();
        m_frameTime = 0;
    }


// update fps information

    void App::update_timer()
    {
        static delta_t counter = 0;
        static int frames = 0;

        m_frameTime = m_timer.elapsed();
        m_runTime += m_frameTime;
        counter += m_frameTime;
        m_timer.start();

        // update fps calculations
        if (counter < 1.0)
        {
            ++frames;
            m_msPerFrameAvg += m_frameTime;
            m_msPerFrameAvg *= .5;
            if (m_frameTime < m_msPerFrameMin)
                m_msPerFrameMin = m_frameTime;
            if (m_frameTime > m_msPerFrameMax)
                m_msPerFrameMax = m_frameTime;
        }
        else
        {
            m_fpsAvg = (frames / counter);
            if (frames < m_fpsMin)
                m_fpsMin = frames;
            if (frames > m_fpsMax)
                m_fpsMax = frames;
            frames = 0;
            --counter;
        }
    }


// window has gained focus

    void App::on_active()
    {
        m_bActive = true;

        if (m_bAutoPause)
        {
        }
    }


// window has lost focus

    void App::on_inactive()
    {
        m_bActive = false;

        if (m_bAutoPause)
        {
        }
    }


// pass glfw mouse/keyboard input callbacks to the main application

    void App::on_input(const input & msg)
    {
        Input(msg);
    }


// return current mouse state

    const mouse_info & App::GetMouse()
    {
        static mouse_info info;
        int i = GLFW_PRESS;
        info.LMB = glfwGetMouseButton(GLFW_MOUSE_BUTTON_LEFT) == i;
        info.MMB = glfwGetMouseButton(GLFW_MOUSE_BUTTON_MIDDLE) == i;
        info.RMB = glfwGetMouseButton(GLFW_MOUSE_BUTTON_RIGHT) == i;
        glfwGetMousePos(&info.x, &info.y);
        return info;
    }


// return current state of specified key

    bool App::GetKey(int code)
    {
        return (glfwGetKey(code) == GLFW_PRESS) ? true : false;
    }


// resize :: glfw callback

    void GLFWCALL ffOnWindowResize(int width, int height)
    {
        g_App.m_window.Resize(width, height);
        GL_DEBUG(glViewport(0, 0, width, height));
    }


// window close :: glfw callback

    int GLFWCALL ffOnWindowClose(void)
    {
        g_App.Quit();
        return 0;
    }


// key event :: glfw callback

    void GLFWCALL ffOnKeyEvent(int key, int action)
    {
        static input msg;
        msg.source = input::ffKeyboard;
        msg.key.code = key;
        msg.key.state = action;
        msg.key.shift = (glfwGetKey(GLFW_KEY_LSHIFT) == GLFW_PRESS) ||
                        (glfwGetKey(GLFW_KEY_RSHIFT) == GLFW_PRESS);
        g_App.on_input(msg);
    }


// text entry :: glfw callback

    void GLFWCALL ffOnText(int ch, int action)
    {
        static input msg;
        msg.source = input::ffText;
        msg.text.code = ch;
        msg.text.state = action;
        g_App.on_input(msg);
    }


// mouse move :: glfw callback

    void GLFWCALL ffOnMouseMove(int x, int y)
    {
        static input msg;
        msg.source = input::ffMouseMove;
        msg.mouse_move.x = x;
        msg.mouse_move.y = y;
        g_App.on_input(msg);
    }


// mouse button :: glfw callback

    void GLFWCALL ffOnMouseButton(int button, int action)
    {
        static input msg;
        msg.source = input::ffMouseButton;
        msg.mouse_button.button = button;
        msg.mouse_button.state = action;
        glfwGetMousePos(&msg.mouse_button.x, &msg.mouse_button.y);
        g_App.on_input(msg);
    }


// mouse wheel :: glfw callback

    void GLFWCALL ffOnMouseWheel(int pos)
    {
        static input msg;
        msg.source = input::ffMouseWheel;
        msg.mouse_wheel.delta = pos;
        glfwGetMousePos(&msg.mouse_wheel.x, &msg.mouse_wheel.y);
        g_App.on_input(msg);
    }

} // exiting namespace ff

////////////////////////////////////////////////////////////////////////
