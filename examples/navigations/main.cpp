
#include <GL/glew.h>

#include <algorithm>
#include <random>
#include <cassert>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include <glow/Error.h>
#include <glow/Uniform.h>
#include <glow/Program.h>
#include <glow/Shader.h>
#include <glow/Buffer.h>
#include <glow/logging.h>
#include <glow/AutoTimer.h>
#include <glow/VertexArrayObject.h>

#include <glowutils/AxisAlignedBoundingBox.h>
#include <glowutils/MathMacros.h>
#include <glowutils/Icosahedron.h>
#include <glowutils/Camera.h>
#include <glowutils/AdaptiveGrid.h>
#include <glowutils/AbstractCoordinateProvider.h>
#include <glowutils/WorldInHandNavigation.h>
#include <glowutils/FlightNavigation.h>
#include <glowutils/FileRegistry.h>
#include <glowutils/File.h>

#include <glowwindow/ContextFormat.h>
#include <glowwindow/Context.h>
#include <glowwindow/Window.h>
#include <glowwindow/WindowEventHandler.h>

using namespace glowwindow;
using namespace glm;


class EventHandler : public WindowEventHandler, glowutils::AbstractCoordinateProvider
{
public:
    EventHandler()
    : m_camera(vec3(0.f, 1.f, 4.0f))
    , m_flightEnabled(false)
    {
        m_aabb.extend(vec3(-8.f, -1.f, -8.f));
        m_aabb.extend(vec3(8.f, 1.f, 8.f));

        m_nav.setCamera(&m_camera);
        m_nav.setCoordinateProvider(this);
        m_nav.setBoundaryHint(m_aabb);

        m_flightNav.setCamera(&m_camera);
    }

    virtual ~EventHandler()
    {
    }

    void createAndSetupTexture();
    void createAndSetupShaders();
	void createAndSetupGeometry();

    virtual void initialize(Window & window) override
    {
        glow::DebugMessageOutput::enable();

        glClearColor(1.0f, 1.0f, 1.0f, 0.f);


        m_sphere = new glow::Program();
        m_sphere->attach(
            glowutils::createShaderFromFile(GL_VERTEX_SHADER,   "data/adaptive-grid/sphere.vert")
        ,   glowutils::createShaderFromFile(GL_FRAGMENT_SHADER, "data/adaptive-grid/sphere.frag"));

        m_icosahedron = new glowutils::Icosahedron(2);
        m_agrid = new glowutils::AdaptiveGrid(16U);

        m_camera.setZNear(0.1f);
        m_camera.setZFar(1024.f);

        m_agrid->setCamera(&m_camera);
    }
    virtual void finalize(Window &) override
    {
        m_sphere = nullptr;
        m_icosahedron = nullptr;
        m_agrid = nullptr;
    }

    virtual void framebufferResizeEvent(ResizeEvent & event) override
    {
        glViewport(0, 0, event.width(), event.height());
        m_camera.setViewport(event.width(), event.height());
    }

    virtual void paintEvent(PaintEvent &) override
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        m_agrid->update();
        m_sphere->setUniform("transform", m_camera.viewProjection());

        m_sphere->use();
        m_icosahedron->draw();
        m_sphere->release();

        m_agrid->draw();
    }

    virtual void idle(Window & window) override
    {
        window.repaint();
    }

    virtual void keyPressEvent(KeyEvent & event)
    {
        switch (event.key())
        {
            case GLFW_KEY_F5:
                glowutils::FileRegistry::instance().reloadAll();
                break;
            case GLFW_KEY_1:
                m_flightEnabled = !m_flightEnabled;
                event.window()->setInputMode(GLFW_CURSOR, m_flightEnabled ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
                break;
            case GLFW_KEY_SPACE:
                m_camera.setCenter(vec3());
                m_camera.setEye(vec3(0.f, 1.f, 4.0f));
                m_camera.setUp(vec3(0,1,0));
                break;
            case GLFW_KEY_F11:
                event.window()->toggleMode();
                break;
        }

        if (!m_flightEnabled)
            return;
        switch (event.key())
        {
            case GLFW_KEY_W:
            case GLFW_KEY_UP:
                m_flightNav.moveForward();
                break;
            case GLFW_KEY_A:
            case GLFW_KEY_LEFT:
                m_flightNav.moveLeft();
                break;
            case GLFW_KEY_S:
            case GLFW_KEY_DOWN:
                m_flightNav.moveBackward();
                break;
            case GLFW_KEY_D:
            case GLFW_KEY_RIGHT:
                m_flightNav.moveRight();
                break;
        }
    }

    virtual void mousePressEvent(MouseEvent & event) override
    {
        if (m_flightEnabled)
            return;

        switch (event.button())
        {
            case GLFW_MOUSE_BUTTON_LEFT:
                m_nav.panBegin(event.pos());
                event.accept();
                break;

            case GLFW_MOUSE_BUTTON_RIGHT:
                m_nav.rotateBegin(event.pos());
                event.accept();
                break;
        }
    }
    virtual void mouseMoveEvent(MouseEvent & event) override
    {
        if (m_flightEnabled)
        {
            m_flightNav.mouseMove((event.pos()-m_lastMousePos));
            m_lastMousePos = event.pos();
            event.accept();
            return;
        }

        switch (m_nav.mode())
        {
        case glowutils::WorldInHandNavigation::PanInteraction:
            m_nav.panProcess(event.pos());
            event.accept();
            break;

        case glowutils::WorldInHandNavigation::RotateInteraction:
            m_nav.rotateProcess(event.pos());
            event.accept();
        }
    }
    virtual void mouseReleaseEvent(MouseEvent & event) override
    {
        if (m_flightEnabled)
            return;

        switch (event.button())
        {
            case GLFW_MOUSE_BUTTON_LEFT:
                m_nav.panEnd();
                event.accept();
                break;

            case GLFW_MOUSE_BUTTON_RIGHT:
                m_nav.rotateEnd();
                event.accept();
                break;
        }
    }

    void scrollEvent(ScrollEvent & event) override
    {
        if (m_flightEnabled)
            return;

        if (glowutils::WorldInHandNavigation::NoInteraction != m_nav.mode())
            return;

        m_nav.scaleAtMouse(event.pos(), -event.offset().y * 0.1f);
        event.accept();
    }

    virtual const float depthAt(const ivec2 & windowCoordinates)
    {
        return AbstractCoordinateProvider::depthAt(m_camera, GL_DEPTH_COMPONENT, windowCoordinates);
    }

    virtual const vec3 objAt(const ivec2 & windowCoordinates)
    {
        return unproject(m_camera, static_cast<GLenum>(GL_DEPTH_COMPONENT), windowCoordinates);
    }

    virtual const vec3 objAt(const ivec2 & windowCoordinates, const float depth)
    {
        return unproject(m_camera, depth, windowCoordinates);
    }

    virtual const glm::vec3 objAt(
        const ivec2 & windowCoordinates
    ,   const float depth
    ,   const mat4 & viewProjectionInverted)
    {
        return unproject(m_camera, viewProjectionInverted, depth, windowCoordinates);
    }

protected:

    glow::ref_ptr<glow::Program> m_sphere;
    
    glow::ref_ptr<glowutils::Icosahedron> m_icosahedron;
    glow::ref_ptr<glowutils::AdaptiveGrid> m_agrid;

    glowutils::Camera m_camera;
    glowutils::WorldInHandNavigation m_nav;
    glowutils::FlightNavigation m_flightNav;
    glm::ivec2 m_lastMousePos;
    bool m_flightEnabled;

    glowutils::AxisAlignedBoundingBox m_aabb;
};


/** This example shows ... .
*/
int main(int argc, char** argv)
{
    ContextFormat format;
    format.setVersion(3, 0);

    Window window;

    window.setEventHandler(new EventHandler());

    if (window.create(format, "Navigations Example"))
    {
        window.context()->setSwapInterval(Context::VerticalSyncronization);

        window.show();

        return MainLoop::run();
    }
    else
    {
        return 1;
    }
}
