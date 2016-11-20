#include <bx/bx.h>
#include <bx/os.h>

#if BX_PLATFORM_LINUX || BX_PLATFORM_BSD
#	define GLFW_EXPOSE_NATIVE_X11
#	define GLFW_EXPOSE_NATIVE_GLX
#elif BX_PLATFORM_OSX
#	define GLFW_EXPOSE_NATIVE_COCOA
#	define GLFW_EXPOSE_NATIVE_NSGL
#elif BX_PLATFORM_WINDOWS
#	define GLFW_EXPOSE_NATIVE_WIN32
#	define GLFW_EXPOSE_NATIVE_WGL
#endif
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <bgfx/bgfx.h>
#include <bgfx/bgfxplatform.h>

#include <bx/uint32_t.h>

struct MyWindow
{
	GLFWwindow* m_GLFWHandle;
	uint32_t m_Width;
	uint32_t m_Height;
	bool m_IsFullscreen;
	bool m_IsMinimized;
	bool m_IsVSyncEnabled;
};

void glfw_errorCallback(int error, const char* desc)
{
	// TODO: 
}

void glfw_keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	MyWindow* win = (MyWindow*)glfwGetWindowUserPointer(window);

	if (key == GLFW_KEY_F1 && action == GLFW_PRESS) {
		static bool showInternalStats = false;
		showInternalStats = !showInternalStats;
		bgfx::setDebug(showInternalStats ? BGFX_DEBUG_STATS : BGFX_DEBUG_TEXT);
	} else if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(win->m_GLFWHandle, GLFW_TRUE);
	}
}

void glfw_textInputCallback(GLFWwindow* window, uint32_t codepoint)
{
	MyWindow* win = (MyWindow*)glfwGetWindowUserPointer(window);

	// TODO: 
}

void glfw_cursorCallback(GLFWwindow* window, double x, double y)
{
	MyWindow* win = (MyWindow*)glfwGetWindowUserPointer(window);

	// TODO: 
}

void glfw_mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	MyWindow* win = (MyWindow*)glfwGetWindowUserPointer(window);

	// TODO: 
}

void glfw_scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	MyWindow* win = (MyWindow*)glfwGetWindowUserPointer(window);

	// TODO: 
}

void glfw_resizeCallback(GLFWwindow* window, int width, int height)
{
	MyWindow* win = (MyWindow*)glfwGetWindowUserPointer(window);

	win->m_IsMinimized = height == 0;
	if (width != 0 && height != 0) {
		bgfx::reset(width, height, win->m_IsVSyncEnabled ? BGFX_RESET_VSYNC : BGFX_RESET_NONE);
		win->m_Width = (uint32_t)width;
		win->m_Height = (uint32_t)height;
	}
}

void glfw_iconifyCallback(GLFWwindow* window, int iconified)
{
	MyWindow* win = (MyWindow*)glfwGetWindowUserPointer(window);

	win->m_IsMinimized = iconified == GLFW_TRUE; // Don't reset bgfx because when the window is restored we don't know the correct window dimensions.
}

bool openGLFWWindow(MyWindow* win, int width, int height, bool fullscreen, bool vsync)
{
	glfwSetErrorCallback(glfw_errorCallback);
	if (!glfwInit()) {
		return false;
	}

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);

	glfwWindowHint(GLFW_DOUBLEBUFFER, 1);
	glfwWindowHint(GLFW_RED_BITS, mode->redBits);
	glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
	glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
	glfwWindowHint(GLFW_ALPHA_BITS, GLFW_DONT_CARE);
	glfwWindowHint(GLFW_DEPTH_BITS, GLFW_DONT_CARE);
	glfwWindowHint(GLFW_STENCIL_BITS, 8);
	glfwWindowHint(GLFW_REFRESH_RATE, GLFW_DONT_CARE);

#if BX_PLATFORM_OSX
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, 1);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif

	GLFWwindow* window = nullptr;
	if (fullscreen) {
		bool nativeResolution = width <= 0 || height <= 0;
		width = nativeResolution ? mode->width : width;
		height = nativeResolution ? mode->height : height;
		window = glfwCreateWindow(width, height, "DLS", monitor, nullptr);
	} else {
		window = glfwCreateWindow(width, height, "DLS", nullptr, nullptr);
		if (!window) {
			return false;
		}

		glfwSetWindowPos(window, (mode->width - width) / 2, (mode->height - height) / 2);
	}

	if (!window) {
		return false;
	}

	if (win->m_GLFWHandle) {
		bgfx::frame();
		bgfx::shutdown();

		glfwDestroyWindow(win->m_GLFWHandle);
	}

	win->m_GLFWHandle = window;
	win->m_Width = (uint32_t)width;
	win->m_Height = (uint32_t)height;
	win->m_IsFullscreen = fullscreen;
	win->m_IsVSyncEnabled = vsync;

	{
		bgfx::PlatformData pd;
#if BX_PLATFORM_LINUX || BX_PLATFORM_BSD
		pd.ndt = glfwGetX11Display();
		pd.nwh = (void*)(uintptr_t)glfwGetX11Window(win->m_GLFWHandle);
#elif BX_PLATFORM_OSX
		pd.ndt = nullptr;
		pd.nwh = glfwGetCocoaWindow(win->m_GLFWHandle);
#elif BX_PLATFORM_WINDOWS
		pd.ndt = nullptr;
		pd.nwh = glfwGetWin32Window(win->m_GLFWHandle);
#endif // BX_PLATFORM_WINDOWS
		pd.context = nullptr;
		pd.backBuffer = nullptr;
		pd.backBufferDS = nullptr;
		bgfx::setPlatformData(pd);
	}

	glfwSetKeyCallback(win->m_GLFWHandle, glfw_keyCallback);
	glfwSetCursorPosCallback(win->m_GLFWHandle, glfw_cursorCallback);
	glfwSetMouseButtonCallback(win->m_GLFWHandle, glfw_mouseButtonCallback);
	glfwSetScrollCallback(win->m_GLFWHandle, glfw_scrollCallback);
	glfwSetWindowSizeCallback(win->m_GLFWHandle, glfw_resizeCallback);
	glfwSetWindowIconifyCallback(win->m_GLFWHandle, glfw_iconifyCallback);
	glfwSetCharCallback(win->m_GLFWHandle, glfw_textInputCallback);
	glfwSetWindowUserPointer(win->m_GLFWHandle, win);

	// Initialize bgfx
	if (!bgfx::init()) {
		return false;
	}

	bgfx::reset((uint32_t)width, (uint32_t)height, vsync ? BGFX_RESET_VSYNC : BGFX_RESET_NONE);
	bgfx::setDebug(BGFX_DEBUG_TEXT);
	bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH | BGFX_CLEAR_STENCIL, 0xffffffff, 1.0f, 0);
	bgfx::setViewSeq(0, true);

	glfwSetTime(0);

	return true;
}

void closeGLFWWindow(MyWindow* win)
{
	if (win->m_GLFWHandle) {
		bgfx::shutdown();
		glfwTerminate();
		win->m_GLFWHandle = nullptr;
	}
}

void onUpdate(float dt)
{
	// TODO: Update your simulation here
}

void onRender(uint32_t winWidth, uint32_t winHeight, uint32_t fbWidth, uint32_t fbHeight, float pxRatio)
{
	bgfx::setViewRect(0, 0, 0, uint16_t(fbWidth), uint16_t(fbHeight));

	// This dummy draw call is here to make sure that view 0 is cleared
	// if no other draw calls are submitted to view 0.
	bgfx::touch(0);

	// Use debug font to print information about this example.
	bgfx::dbgTextClear();
	bgfx::dbgTextPrintf(0, 1, 0x4f, "bgfxTemplate");
	bgfx::dbgTextPrintf(0, 2, 0x6f, "Description: Minimal bgfx + GLFW application.");
	bgfx::dbgTextPrintf(0, 4, 0x4f, "Press F1 to toggle bgfx stats, Esc to quit");
}

int main()
{
	MyWindow win;
	memset(&win, 0, sizeof(MyWindow));

	if (!openGLFWWindow(&win, 1280, 720, false, true)) {
		return -1;
	}

	// Main loop
	double prevTime = glfwGetTime();
	while (!glfwWindowShouldClose(win.m_GLFWHandle)) {
		double t = glfwGetTime();
		double dt = t - prevTime;
		prevTime = t;

		// If the window is minimized, sleep for 1 frame.
		if (win.m_IsMinimized) {
			bx::sleep(16);
			glfwPollEvents();
			continue;
		}

		int winWidth, winHeight;
		int fbWidth, fbHeight;
		glfwGetWindowSize(win.m_GLFWHandle, &winWidth, &winHeight);
		glfwGetFramebufferSize(win.m_GLFWHandle, &fbWidth, &fbHeight);

		winWidth = bx::uint32_max(winWidth, 1);
		winHeight = bx::uint32_max(winHeight, 1);
		fbWidth = bx::uint32_max(fbWidth, 1);
		fbHeight = bx::uint32_max(fbHeight, 1);
		
		// Update and render
		onUpdate((float)dt);
		onRender(winWidth, winHeight, fbWidth, fbHeight, (float)fbWidth / (float)winWidth);

		// Advance to next frame.
		bgfx::frame();
		glfwPollEvents();
	}

	closeGLFWWindow(&win);

	return 0;
}
