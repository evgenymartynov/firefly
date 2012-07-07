#ifndef FIREFLY_APP_HPP
#define FIREFLY_APP_HPP

#include <firefly/common.hpp>
#include <firefly/core/input.hpp>
#include <firefly/core/window.hpp>
#include <firefly/core/videomode.hpp>
#include <firefly/core/timer.hpp>
#include <firefly/core/singleton.hpp>

#define FF_EXIT_SUCCESS 0
#define FF_EXIT_FAILURE -1
#define FF_CONFIG_FILE  "firefly.ini"
#define FF_LOG_FILE     "firefly.log"

// forward declaration of main
int main(int,char**);

////////////////////////////////////////////////////////////////////////

namespace ff {

// glfw callback functions

    void GLFWCALL ffOnWindowResize(int width, int height);
    int  GLFWCALL ffOnWindowClose();
    void GLFWCALL ffOnKeyEvent(int key, int action);
    void GLFWCALL ffOnText(int ch, int action);
    void GLFWCALL ffOnMouseMove(int x, int y);
    void GLFWCALL ffOnMouseButton(int button, int action);
    void GLFWCALL ffOnMouseWheel(int pos);

// main application class

    class App : public singleton<App> {

        // ctor / dtor / main
        App();
        ~App();
        friend int ::main(int,char**);

        // private members
        Window    m_window;
        timer     m_timer;
        int       m_numProcessors;
        string    m_appTitle;
        bool      m_bRunning;
        bool      m_bActive;
        bool      m_bAutoPause;
        delta_t   m_frameTime;
        delta_t   m_gameTime;
        delta_t   m_runTime;
        delta_t   m_fpsAvg;
        delta_t   m_fpsMin;
        delta_t   m_fpsMax;
        delta_t   m_msPerFrameAvg;
        delta_t   m_msPerFrameMin;
        delta_t   m_msPerFrameMax;

    public:

        // state-based app functions
        bool Load();
        bool Exit();
        void Update(const delta_t dt, const delta_t elapsed);
        void Render(const delta_t dt, const delta_t elapsed);

        // event-based app functions
        void Input(const input & msg);
        void Resize(int width, int height);

        // app state-change functions
        void Quit() { m_bRunning = false; }

        // app helper functions
        void Log(const char * format, ...);

        // getters
        int GetWidth() const { return m_window.GetWidth(); }
        int GetHeight() const { return m_window.GetHeight(); }
        bool IsActive() const { return m_bActive; }
        bool IsRunning() const { return m_bRunning; }
        delta_t GetFrameTime() const { return m_frameTime; }
        delta_t GetGameTime() const { return m_gameTime; }
        delta_t GetRunTime() const { return m_runTime; }
        delta_t GetFPS() const { return m_fpsAvg; }
        bool GetKey(int code);
        const mouse_info & GetMouse();

        // setters
        void SetWindowTitle(const char * title, ...);

    private:

        // app window functions
        void configure_app();
        bool load_config(const string & filePath,
                         WindowSettings & ws,
                         GLContext & glc,
                         VideoMode & vm);

        // app loop functions
        bool init();
        void main_loop();
        void shutdown();

        // main loop functions
        void frame_update(const delta_t dt, const delta_t elapsed);
        void frame_render(const delta_t dt, const delta_t elapsed);
        void update_timer();

        // app event handlers
        void on_active();
        void on_inactive();
        void on_input(const input & msg);

        // glfw callback functions
        friend void GLFWCALL ffOnWindowResize(int width, int height);
        friend int  GLFWCALL ffOnWindowClose();
        friend void GLFWCALL ffOnKeyEvent(int key, int action);
        friend void GLFWCALL ffOnText(int ch, int action);
        friend void GLFWCALL ffOnMouseMove(int x, int y);
        friend void GLFWCALL ffOnMouseButton(int button, int action);
        friend void GLFWCALL ffOnMouseWheel(int pos);
    };

#define g_App ff::App::get_singleton()

} // exiting namespace ff

////////////////////////////////////////////////////////////////////////

#endif
