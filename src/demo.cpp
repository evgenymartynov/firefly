#include <firefly.hpp>
#include <firefly/graphics/shader.hpp>

////////////////////////////////////////////////////////////////////////

namespace ff {

    bool App::Load()
    {
        GL_DEBUG(glClearDepth(1.f));
        GL_DEBUG(glClearColor(.2f, .2f, .2f, .2f));
        GL_DEBUG(glDepthMask(GL_TRUE));
        GL_DEBUG(glDepthFunc(GL_LEQUAL));
        GL_DEBUG(glCullFace(GL_BACK));
        GL_DEBUG(glFrontFace(GL_CCW));

        g_Shader.Compile("shader");
        return true;
    }

    bool App::Exit()
    {
        return true;
    }

    void App::Update(const delta_t dt, const delta_t elapsed)
    {
        mouse_info mi = GetMouse();
        SetWindowTitle("firefly-demo v%d.%d (FPS: %.2lf)"
                       " x-%d y-%d",
                       FF_MAJOR_VERSION, FF_MINOR_VERSION,
                       m_fpsAvg, mi.x, mi.y);
    }

    void App::Render(const delta_t dt, const delta_t elapsed)
    {
        GL_DEBUG(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
    }

    void App::Input(const input & msg)
    {
        if (msg.source == input::ffKeyboard)
        {
            switch(msg.key.code)
            {
            case GLFW_KEY_ESC:
                Quit();
                break;
            }
        }
    }

} // exiting namespace ff

////////////////////////////////////////////////////////////////////////
