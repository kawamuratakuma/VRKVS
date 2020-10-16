#include "Screen.h"
#include "InputDevice.h"
#include "ImGuiWidgetHandler.h"
#include "ImGuiWindow.h"
#include "ImGuiWidget.h"
#include "imgui.h"
#include "imgui_impl_opengl2.h"
#include "imgui_impl_glut.h"
#include "imgui_internal.h"

#include <vector>
#include <algorithm>

kvs::oculus::jaea::ImGuiWidgetHandler::ImGuiWidgetHandler(kvs::oculus::jaea::Screen *screen, int w, int h) :
        kvs::oculus::jaea::WidgetHandler(w, h), m_input_device(screen->headMountedDisplay()), m_prev_trigger_state_left(0.0f), m_prev_trigger_state_right(
                0.0f), focusIndex(0) {
    m_screen = screen;
    screen->setWidgetHandler(this);
}

bool kvs::oculus::jaea::ImGuiWidgetHandler::initialize() {
// #ifdef USE_OPENGL3
//  if(glewInit() != GLEW_OK)
//  {
//      return false;
//  }
// #endif

//m_input_device = m_screen->headMountedDisplay();

    IMGUI_CHECKVERSION();
    ImGuiContext *context = ImGui::CreateContext();
    GImGui->NavDisableHighlight = false;

#ifdef DEBUG_HUD
    std::cerr << "ImGuiWidgetHandler::initialize() : ImGui::CreateContext() finished. (context=" << context << ")" << std::endl;
#endif //DEBUG_HUD

    ImGuiIO& io = ImGui::GetIO();
    (void) io; // ???

#ifdef DEBUG_HUD
    std::cerr << "ImGuiWidgetHandler::initialize() : ImGui::GetIO()() finished." << std::endl;
#endif //DEBUG_HUD

    ImGui::StyleColorsDark();
    ImGui_ImplGLUT_Init();

#ifdef DEBUG_HUD
    std::cerr << "ImGuiWidgetHandler::initialize() : ImGui_ImplGLUT_Init() finished." << std::endl;
#endif //DEBUG_HUD

// #ifdef USE_OPENGL3
//     ImGui_ImplOpenGL3_Init();
// #else
    ImGui_ImplOpenGL2_Init();
//#endif

#ifdef DEBUG_HUD
    std::cerr << "ImGuiWidgetHandler::initialize() : ImGui_ImplOpenGL2_Init() finished." << std::endl;
#endif //DEBUG_HUD

    prev_buttons = 0;

    return true;
}

bool kvs::oculus::jaea::ImGuiWidgetHandler::destroy() {
#ifdef USE_OPENGL3
    ImGui_ImplOpenGL3_Shutdown();
#else
    ImGui_ImplOpenGL2_Shutdown();
#endif
    ImGui_ImplGLUT_Shutdown();
    ImGui::DestroyContext();
    return true;
}

void kvs::oculus::jaea::ImGuiWidgetHandler::update() {
#ifdef DEBUG_HUD
    std::cerr << "ImGuiWidgetHandler::update(): start" << std::endl;
#endif // DEBUG_HUD

    ImGuiIO &io = ImGui::GetIO();
    //io.DisplaySize.x = this->m_width;
    //io.DisplaySize.y = this->m_height;

    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.BackendFlags |= ImGuiBackendFlags_HasGamepad;
    io.NavActive = 1;
    io.NavVisible = 1;

    // set Touch Controller buttons(A/B/X/Y)
    // button layout is based on settings for XBox.
    // WARNING!! input_state.Touches means "Button Touch" not "Button Push/Release"!
    ovrInputState input_state = m_input_device.inputState(ovrControllerType_Touch);
    int buttons = input_state.Buttons;

    if ((buttons & ovrButton_A) != 0) {
        io.NavInputs[ImGuiNavInput_Activate] = 1.0f;
    }
    if ((buttons & ovrButton_B) != 0) {
        io.NavInputs[ImGuiNavInput_Cancel] = 1.0f;
    }
    if ((buttons & ovrButton_Y) != 0) {
        if ((prev_buttons & ovrButton_Y) == 0) {
            this->setPrevWindowFocus();
        }
    }
    if ((buttons & ovrButton_X) != 0) {
        if ((prev_buttons & ovrButton_X) == 0) {
            this->setNextWindowFocus();
        }
    }

    // this class handles left thumb stick as d-pad
    ovrVector2f dpad = input_state.Thumbstick[ovrHand_Left];
    io.NavInputs[ImGuiNavInput_DpadLeft] = 0.0f;
    io.NavInputs[ImGuiNavInput_DpadRight] = 0.0f;
    io.NavInputs[ImGuiNavInput_DpadUp] = 0.0f;
    io.NavInputs[ImGuiNavInput_DpadDown] = 0.0f;
    if(dpad.x < -0.5f && prev_dpad.x > -0.5f) {
        io.NavInputs[ImGuiNavInput_DpadLeft] = 1.0f;
    } else if(dpad.x > 0.5f && prev_dpad.x < 0.5f) {
        io.NavInputs[ImGuiNavInput_DpadRight] = 1.0f;
    }
    if(dpad.y < -0.5f && prev_dpad.y > -0.5f) {
        io.NavInputs[ImGuiNavInput_DpadDown] = 1.0f;
    } else if(dpad.y > 0.5f && prev_dpad.y < 0.5f) {
        io.NavInputs[ImGuiNavInput_DpadUp] = 1.0f;
    }

    // right thumb stick is used to update values
    ovrVector2f inputStick = input_state.Thumbstick[ovrHand_Right];
    io.NavInputs[ImGuiNavInput_LStickDown] = 0.0f;
    io.NavInputs[ImGuiNavInput_LStickUp] = 0.0f;
    io.NavInputs[ImGuiNavInput_LStickLeft] = 0.0f;
    io.NavInputs[ImGuiNavInput_LStickRight] = 0.0f;
    if (inputStick.y < -0.1f) {
        io.NavInputs[ImGuiNavInput_LStickDown] = (inputStick.y - (-0.1f)) / -0.9f;
    } else if (inputStick.y > 0.1f) {
        io.NavInputs[ImGuiNavInput_LStickUp] = (inputStick.y - (0.1f)) / 0.9f;
    }
    if (inputStick.x < -0.1f) {
        io.NavInputs[ImGuiNavInput_LStickLeft] = (inputStick.x - (-0.1f)) / -0.9f;
    } else if (inputStick.x > 0.1f) {
        io.NavInputs[ImGuiNavInput_LStickRight] = (inputStick.x - (0.1f)) / 0.9f;
    }

    prev_buttons = buttons;
    prev_dpad = dpad;
    prev_inputStick = inputStick;

    // draw widgets
    io.DisplaySize.x = this->m_width;
    io.DisplaySize.y = this->m_height;

#ifdef DEBUG_HUD
    std::cerr << "ImGuiWidgetHandler::update(): ImGui::GetIO() -> DisplaySize=(" << io.DisplaySize.x << ", " << io.DisplaySize.y << ")" << std::endl;
#endif // DEBUG_HUD

    // @see : imgui/examples/example_glut_opengl2/main.cpp
#ifdef USE_OPENGL3
    ImGui_ImplOpenGL3_NewFrame();
#else
    ImGui_ImplOpenGL2_NewFrame();
#endif // USE_OPENGL3

#ifdef DEBUG_HUD
    std::cerr << "ImGuiWidgetHandler::update(): ImGui_ImplOpenGL2_NewFrame()" << std::endl;
#endif

    ImGui::NewFrame();

#ifdef DEBUG_HUD
    std::cerr << "ImGuiWidgetHandler::update(): ImGui::NewFrame()" << std::endl;
#endif

    for (int i = 0; i < m_windows.size(); i++) {
        ImGuiWindow *window = m_windows[i];

#ifdef DEBUG_HUD
        std::cerr << "draw Window "<< i << std::endl;
#endif // DEBUG_HUD

        //window->setFocused(i == focusIndex);
        if (window->isVisible()) {
            window->draw();
        }
    }
    ImGui::EndFrame();

#ifdef DEBUG_HUD
    std::cerr << "ImGuiWidgetHandler::update(): ImGui::EndFrame() finished." << std::endl;
#endif // DEBUG_HUD

    ImGui::Render();

#ifdef DEBUG_HUD
    std::cerr << "ImGuiWidgetHandler::update(): ImGui::Render() finished." << std::endl;
#endif // DEBUG_HUD

    //ImGuiIO &io = ImGui::GetIO();

// #ifdef DEBUG_HUD
//     std::cerr << "HUDTest01ImGuiHandler::update(): ImGui::GetIO() -> DisplaySize=(" << io.DisplaySize.x << ", " << io.DisplaySize.y << ")" << std::endl;
// #endif // DEBUG_HUD

    glViewport(0, 0, (GLsizei) io.DisplaySize.x, (GLsizei) io.DisplaySize.y);

#ifdef DEBUG_HUD
    std::cerr << "ImGuiWidgetHandler::update(): glViewport() finished." << std::endl;
#endif // DEBUG_HUD

#ifdef USE_OPENGL3
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#else
    ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
#endif

#ifdef DEBUG_HUD
    std::cerr << "ImGuiWidgetHandler::update(): ImGui_ImplOpenGL*_RenderDrawData() finished." << std::endl;
#endif // DEBUG_HUD

}

int kvs::oculus::jaea::ImGuiWidgetHandler::indexOf(ImGuiWindow *window) {
    if (window == nullptr) {
        return -1;
    }
    for (size_t i = 0; i < m_windows.size(); i++) {
        if (m_windows[i] == window) {
            return (int) i;
        }
    }
    return -1;
}

void kvs::oculus::jaea::ImGuiWidgetHandler::addWindow(ImGuiWindow *window) {
    if (window == nullptr) {
        return;
    }
    if (indexOf(window) < 0) {
        m_windows.push_back(window);
    }
}
void kvs::oculus::jaea::ImGuiWidgetHandler::removeWindow(ImGuiWindow *window) {
    removeWindow(indexOf(window));
}
void kvs::oculus::jaea::ImGuiWidgetHandler::removeWindow(int index) {
    if (index < 0 || m_windows.size() <= (size_t) index) {
        return;
    }

    m_windows.erase(m_windows.begin() + index);
}

int kvs::oculus::jaea::ImGuiWidgetHandler::getWindowFocusIndex() {
    return focusIndex;
}
void kvs::oculus::jaea::ImGuiWidgetHandler::setWindowFocus(ImGuiWindow *window) {
    setWindowFocus(indexOf(window));
}
void kvs::oculus::jaea::ImGuiWidgetHandler::setWindowFocus(int index) {
    if (index < 0) {
        focusIndex = 0;
    } else if (index >= m_windows.size()) {
        focusIndex = m_windows.size() - 1;
    } else {
        focusIndex = index;
    }

    for (int i = 0; i < m_windows.size(); i++) {
        m_windows[i]->setFocused(i == focusIndex);
    }
}
void kvs::oculus::jaea::ImGuiWidgetHandler::setNextWindowFocus() {
    focusIndex++;
    if (focusIndex >= m_windows.size()) {
        focusIndex = 0;
    }
    setWindowFocus(focusIndex);
}
void kvs::oculus::jaea::ImGuiWidgetHandler::setPrevWindowFocus() {
    focusIndex--;
    if (focusIndex < 0) {
        focusIndex = m_windows.size() - 1;
    }
    setWindowFocus(focusIndex);
}
