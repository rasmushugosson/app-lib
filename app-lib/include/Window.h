#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <stdint.h>
#include <string>
#include <bitset>
#include <functional>

#include "Log.h"

namespace ae
{
	enum class GraphicsAPI
	{
		OPENGL = 0,
		VULKAN
	};

	struct WindowDesc
	{
		std::string title;
		uint32_t width;
		uint32_t height;
		bool resizable;
		bool minimizable;
		bool minimized;
		bool maximizable;
		bool maximized;
		bool fullscreen;
		uint8_t monitor;
		bool vsync;
		uint32_t fps;
		GraphicsAPI graphicsAPI;

		WindowDesc()
			: title("Untitled")
			, width(1280)
			, height(720)
			, resizable(true)
			, minimizable(true)
			, minimized(false)
			, maximizable(true)
			, maximized(false)
			, fullscreen(false)
			, monitor(0)
			, vsync(true)
			, fps(60)
			, graphicsAPI(GraphicsAPI::OPENGL)
		{
		};
	};

	class Window;

#define AE_KEYBOARD_SPACE              32
#define AE_KEYBOARD_APOSTROPHE         39
#define AE_KEYBOARD_COMMA              44
#define AE_KEYBOARD_MINUS              45
#define AE_KEYBOARD_PERIOD             46
#define AE_KEYBOARD_SLASH              47
#define AE_KEYBOARD_0                  48
#define AE_KEYBOARD_1                  49
#define AE_KEYBOARD_2                  50
#define AE_KEYBOARD_3                  51
#define AE_KEYBOARD_4                  52
#define AE_KEYBOARD_5                  53
#define AE_KEYBOARD_6                  54
#define AE_KEYBOARD_7                  55
#define AE_KEYBOARD_8                  56
#define AE_KEYBOARD_9                  57
#define AE_KEYBOARD_SEMICOLON          59
#define AE_KEYBOARD_EQUAL              61
#define AE_KEYBOARD_A                  65
#define AE_KEYBOARD_B                  66
#define AE_KEYBOARD_C                  67
#define AE_KEYBOARD_D                  68
#define AE_KEYBOARD_E                  69
#define AE_KEYBOARD_F                  70
#define AE_KEYBOARD_G                  71
#define AE_KEYBOARD_H                  72
#define AE_KEYBOARD_I                  73
#define AE_KEYBOARD_J                  74
#define AE_KEYBOARD_K                  75
#define AE_KEYBOARD_L                  76
#define AE_KEYBOARD_M                  77
#define AE_KEYBOARD_N                  78
#define AE_KEYBOARD_O                  79
#define AE_KEYBOARD_P                  80
#define AE_KEYBOARD_Q                  81
#define AE_KEYBOARD_R                  82
#define AE_KEYBOARD_S                  83
#define AE_KEYBOARD_T                  84
#define AE_KEYBOARD_U                  85
#define AE_KEYBOARD_V                  86
#define AE_KEYBOARD_W                  87
#define AE_KEYBOARD_X                  88
#define AE_KEYBOARD_Y                  89
#define AE_KEYBOARD_Z                  90
#define AE_KEYBOARD_LEFT_BRACKET       91
#define AE_KEYBOARD_BACKSLASH          92
#define AE_KEYBOARD_RIGHT_BRACKET      93
#define AE_KEYBOARD_GRAVE_ACCENT       96
#define AE_KEYBOARD_WORLD_1            161
#define AE_KEYBOARD_WORLD_2            162

#define AE_KEYBOARD_ESCAPE             256
#define AE_KEYBOARD_ENTER              257
#define AE_KEYBOARD_TAB                258
#define AE_KEYBOARD_BACKSPACE          259
#define AE_KEYBOARD_INSERT             260
#define AE_KEYBOARD_DELETE             261
#define AE_KEYBOARD_RIGHT              262
#define AE_KEYBOARD_LEFT               263
#define AE_KEYBOARD_DOWN               264
#define AE_KEYBOARD_UP                 265
#define AE_KEYBOARD_PAGE_UP            266
#define AE_KEYBOARD_PAGE_DOWN          267
#define AE_KEYBOARD_HOME               268
#define AE_KEYBOARD_END                269
#define AE_KEYBOARD_CAPS_LOCK          280
#define AE_KEYBOARD_SCROLL_LOCK        281
#define AE_KEYBOARD_NUM_LOCK           282
#define AE_KEYBOARD_PRINT_SCREEN       283
#define AE_KEYBOARD_PAUSE              284
#define AE_KEYBOARD_F1                 290
#define AE_KEYBOARD_F2                 291
#define AE_KEYBOARD_F3                 292
#define AE_KEYBOARD_F4                 293
#define AE_KEYBOARD_F5                 294
#define AE_KEYBOARD_F6                 295
#define AE_KEYBOARD_F7                 296
#define AE_KEYBOARD_F8                 297
#define AE_KEYBOARD_F9                 298
#define AE_KEYBOARD_F10                299
#define AE_KEYBOARD_F11                300
#define AE_KEYBOARD_F12                301
#define AE_KEYBOARD_F13                302
#define AE_KEYBOARD_F14                303
#define AE_KEYBOARD_F15                304
#define AE_KEYBOARD_F16                305
#define AE_KEYBOARD_F17                306
#define AE_KEYBOARD_F18                307
#define AE_KEYBOARD_F19                308
#define AE_KEYBOARD_F20                309
#define AE_KEYBOARD_F21                310
#define AE_KEYBOARD_F22                311
#define AE_KEYBOARD_F23                312
#define AE_KEYBOARD_F24                313
#define AE_KEYBOARD_F25                314
#define AE_KEYBOARD_KP_0               320
#define AE_KEYBOARD_KP_1               321
#define AE_KEYBOARD_KP_2               322
#define AE_KEYBOARD_KP_3               323
#define AE_KEYBOARD_KP_4               324
#define AE_KEYBOARD_KP_5               325
#define AE_KEYBOARD_KP_6               326
#define AE_KEYBOARD_KP_7               327
#define AE_KEYBOARD_KP_8               328
#define AE_KEYBOARD_KP_9               329
#define AE_KEYBOARD_KP_DECIMAL         330
#define AE_KEYBOARD_KP_DIVIDE          331
#define AE_KEYBOARD_KP_MULTIPLY        332
#define AE_KEYBOARD_KP_SUBTRACT        333
#define AE_KEYBOARD_KP_ADD             334
#define AE_KEYBOARD_KP_ENTER           335
#define AE_KEYBOARD_KP_EQUAL           336
#define AE_KEYBOARD_LEFT_SHIFT         340
#define AE_KEYBOARD_LEFT_CONTROL       341
#define AE_KEYBOARD_LEFT_ALT           342
#define AE_KEYBOARD_LEFT_SUPER         343
#define AE_KEYBOARD_RIGHT_SHIFT        344
#define AE_KEYBOARD_RIGHT_CONTROL      345
#define AE_KEYBOARD_RIGHT_ALT          346
#define AE_KEYBOARD_RIGHT_SUPER        347
#define AE_KEYBOARD_MENU               348

#undef DELETE

	enum class Key
	{
		SPACE = AE_KEYBOARD_SPACE,
		APOSTROPHE = AE_KEYBOARD_APOSTROPHE,
		COMMA = AE_KEYBOARD_COMMA,
		MINUS = AE_KEYBOARD_MINUS,
		PERIOD = AE_KEYBOARD_PERIOD,
		SLASH = AE_KEYBOARD_SLASH,
		ZERO = AE_KEYBOARD_0,
		ONE = AE_KEYBOARD_1,
		TWO = AE_KEYBOARD_2,
		THREE = AE_KEYBOARD_3,
		FOUR = AE_KEYBOARD_4,
		FIVE = AE_KEYBOARD_5,
		SIX = AE_KEYBOARD_6,
		SEVEN = AE_KEYBOARD_7,
		EIGHT = AE_KEYBOARD_8,
		NINE = AE_KEYBOARD_9,
		SEMICOLON = AE_KEYBOARD_SEMICOLON,
		EQUAL = AE_KEYBOARD_EQUAL,
		A = AE_KEYBOARD_A,
		B = AE_KEYBOARD_B,
		C = AE_KEYBOARD_C,
		D = AE_KEYBOARD_D,
		E = AE_KEYBOARD_E,
		F = AE_KEYBOARD_F,
		G = AE_KEYBOARD_G,
		H = AE_KEYBOARD_H,
		I = AE_KEYBOARD_I,
		J = AE_KEYBOARD_J,
		K = AE_KEYBOARD_K,
		L = AE_KEYBOARD_L,
		M = AE_KEYBOARD_M,
		N = AE_KEYBOARD_N,
		O = AE_KEYBOARD_O,
		P = AE_KEYBOARD_P,
		Q = AE_KEYBOARD_Q,
		R = AE_KEYBOARD_R,
		S = AE_KEYBOARD_S,
		T = AE_KEYBOARD_T,
		U = AE_KEYBOARD_U,
		V = AE_KEYBOARD_V,
		W = AE_KEYBOARD_W,
		X = AE_KEYBOARD_X,
		Y = AE_KEYBOARD_Y,
		Z = AE_KEYBOARD_Z,
		LEFT_BRACKET = AE_KEYBOARD_LEFT_BRACKET,
		BACKSLASH = AE_KEYBOARD_BACKSLASH,
		RIGHT_BRACKET = AE_KEYBOARD_RIGHT_BRACKET,
		GRAVE_ACCENT = AE_KEYBOARD_GRAVE_ACCENT,
		WORLD_1 = AE_KEYBOARD_WORLD_1,
		WORLD_2 = AE_KEYBOARD_WORLD_2,
		ESCAPE = AE_KEYBOARD_ESCAPE,
		ENTER = AE_KEYBOARD_ENTER,
		TAB = AE_KEYBOARD_TAB,
		BACKSPACE = AE_KEYBOARD_BACKSPACE,
		INSERT = AE_KEYBOARD_INSERT,
		DELETE = AE_KEYBOARD_DELETE,
		RIGHT = AE_KEYBOARD_RIGHT,
		LEFT = AE_KEYBOARD_LEFT,
		DOWN = AE_KEYBOARD_DOWN,
		UP = AE_KEYBOARD_UP,
		PAGE_UP = AE_KEYBOARD_PAGE_UP,
		PAGE_DOWN = AE_KEYBOARD_PAGE_DOWN,
		HOME = AE_KEYBOARD_HOME,
		END = AE_KEYBOARD_END,
		CAPS_LOCK = AE_KEYBOARD_CAPS_LOCK,
		SCROLL_LOCK = AE_KEYBOARD_SCROLL_LOCK,
		NUM_LOCK = AE_KEYBOARD_NUM_LOCK,
		PRINT_SCREEN = AE_KEYBOARD_PRINT_SCREEN,
		PAUSE = AE_KEYBOARD_PAUSE,
		F1 = AE_KEYBOARD_F1,
		F2 = AE_KEYBOARD_F2,
		F3 = AE_KEYBOARD_F3,
		F4 = AE_KEYBOARD_F4,
		F5 = AE_KEYBOARD_F5,
		F6 = AE_KEYBOARD_F6,
		F7 = AE_KEYBOARD_F7,
		F8 = AE_KEYBOARD_F8,
		F9 = AE_KEYBOARD_F9,
		F10 = AE_KEYBOARD_F10,
		F11 = AE_KEYBOARD_F11,
		F12 = AE_KEYBOARD_F12,
		F13 = AE_KEYBOARD_F13,
		F14 = AE_KEYBOARD_F14,
		F15 = AE_KEYBOARD_F15,
		F16 = AE_KEYBOARD_F16,
		F17 = AE_KEYBOARD_F17,
		F18 = AE_KEYBOARD_F18,
		F19 = AE_KEYBOARD_F19,
		F20 = AE_KEYBOARD_F20,
		F21 = AE_KEYBOARD_F21,
		F22 = AE_KEYBOARD_F22,
		F23 = AE_KEYBOARD_F23,
		F24 = AE_KEYBOARD_F24,
		F25 = AE_KEYBOARD_F25,
		KP_0 = AE_KEYBOARD_KP_0,
		KP_1 = AE_KEYBOARD_KP_1,
		KP_2 = AE_KEYBOARD_KP_2,
		KP_3 = AE_KEYBOARD_KP_3,
		KP_4 = AE_KEYBOARD_KP_4,
		KP_5 = AE_KEYBOARD_KP_5,
		KP_6 = AE_KEYBOARD_KP_6,
		KP_7 = AE_KEYBOARD_KP_7,
		KP_8 = AE_KEYBOARD_KP_8,
		KP_9 = AE_KEYBOARD_KP_9,
		KP_DECIMAL = AE_KEYBOARD_KP_DECIMAL,
		KP_DIVIDE = AE_KEYBOARD_KP_DIVIDE,
		KP_MULTIPLY = AE_KEYBOARD_KP_MULTIPLY,
		KP_SUBTRACT = AE_KEYBOARD_KP_SUBTRACT,
		KP_ADD = AE_KEYBOARD_KP_ADD,
		KP_ENTER = AE_KEYBOARD_KP_ENTER,
		KP_EQUAL = AE_KEYBOARD_KP_EQUAL,
		LEFT_SHIFT = AE_KEYBOARD_LEFT_SHIFT,
		LEFT_CONTROL = AE_KEYBOARD_LEFT_CONTROL,
		LEFT_ALT = AE_KEYBOARD_LEFT_ALT,
		LEFT_SUPER = AE_KEYBOARD_LEFT_SUPER,
		RIGHT_SHIFT = AE_KEYBOARD_RIGHT_SHIFT,
		RIGHT_CONTROL = AE_KEYBOARD_RIGHT_CONTROL,
		RIGHT_ALT = AE_KEYBOARD_RIGHT_ALT,
		RIGHT_SUPER = AE_KEYBOARD_RIGHT_SUPER,
		MENU = AE_KEYBOARD_MENU
	};

	class Keyboard
	{
	public:
		Keyboard();
		~Keyboard();

		bool IsKeyPressed(int32_t key) const;
		bool IsKeyPressed(Key key) const;

		std::string GetTyped();
	private:
		void SetKeyPressed(int32_t key, bool pressed);
		void SetKeyTyped(int32_t key);
	private:
		std::bitset<GLFW_KEY_LAST + 1> m_Keys;
		std::ostringstream m_Typed;

		friend class Window;
	};

#define AE_MOUSE_BUTTON_1				0
#define AE_MOUSE_BUTTON_2				1
#define AE_MOUSE_BUTTON_3				2
#define AE_MOUSE_BUTTON_4				3
#define AE_MOUSE_BUTTON_5				4
#define AE_MOUSE_BUTTON_6				5
#define AE_MOUSE_BUTTON_7				6
#define AE_MOUSE_BUTTON_8				7

#define AE_MOUSE_BUTTON_LEFT      AE_MOUSE_BUTTON_1
#define AE_MOUSE_BUTTON_RIGHT     AE_MOUSE_BUTTON_2
#define AE_MOUSE_BUTTON_MIDDLE    AE_MOUSE_BUTTON_3

	enum class MouseButton
	{
		BUTTON_LEFT = AE_MOUSE_BUTTON_1,
		BUTTON_RIGHT = AE_MOUSE_BUTTON_2,
		BUTTON_MIDDLE = AE_MOUSE_BUTTON_3,
		BUTTON_4 = AE_MOUSE_BUTTON_4,
		BUTTON_5 = AE_MOUSE_BUTTON_5,
		BUTTON_6 = AE_MOUSE_BUTTON_6,
		BUTTON_7 = AE_MOUSE_BUTTON_7,
		BUTTON_8 = AE_MOUSE_BUTTON_8
	};

	class Mouse
	{
	public:
		Mouse(Window* pWindow);
		~Mouse();

		bool IsButtonPressed(int32_t button) const;
		bool IsButtonPressed(MouseButton button) const;

		float GetX() const;
		float GetY() const;

		void SetX(float x);
		void SetY(float y);

		void SetPosition(float x, float y);

		inline float GetDeltaX()
		{
			float x = m_DeltaX;
			m_DeltaX = 0.0f;
			return x;
		};

		inline float GetDeltaY()
		{
			float y = m_DeltaY;
			m_DeltaY = 0.0f;
			return y;
		};

		inline float GetScrollX()
		{
			float x = m_ScrollX;
			m_ScrollX = 0.0f;
			return x;
		};

		inline float GetScrollY()
		{
			float y = m_ScrollY;
			m_ScrollY = 0.0f;
			return y;
		};

		inline bool IsEntered() const { return m_Entered; }
	private:
		void SetPressed(int32_t button, bool pressed);
		void SetMoved(float x, float y);
		void SetScrolled(float x, float y);
		void SetEntered(bool entered);
	private:
		Window* m_pWindow;
		std::bitset<GLFW_MOUSE_BUTTON_LAST + 1> m_Buttons;
		float m_DeltaX;
		float m_DeltaY;
		float m_ScrollX;
		float m_ScrollY;
		bool m_Entered;

		friend class Window;
	};

#define AE_CONTROLLER_BUTTON_A               0
#define AE_CONTROLLER_BUTTON_B               1
#define AE_CONTROLLER_BUTTON_X               2
#define AE_CONTROLLER_BUTTON_Y               3
#define AE_CONTROLLER_BUTTON_TRIANGLE        AE_CONTROLLER_BUTTON_Y
#define AE_CONTROLLER_BUTTON_CIRCLE          AE_CONTROLLER_BUTTON_B
#define AE_CONTROLLER_BUTTON_SQUARE          AE_CONTROLLER_BUTTON_X
#define AE_CONTROLLER_BUTTON_CROSS           AE_CONTROLLER_BUTTON_A
#define AE_CONTROLLER_BUTTON_LEFT_BUMPER     4
#define AE_CONTROLLER_BUTTON_RIGHT_BUMPER    5
#define AE_CONTROLLER_R1					 AE_CONTROLLER_BUTTON_RIGHT_BUMPER	
#define AE_CONTROLLER_L1					 AE_CONTROLLER_BUTTON_LEFT_BUMPER
#define AE_CONTROLLER_BUTTON_BACK            6
#define AE_CONTROLLER_BUTTON_START           7
#define AE_CONTROLLER_BUTTON_GUIDE           8
#define AE_CONTROLLER_BUTTON_LEFT_THUMB      9
#define AE_CONTROLLER_BUTTON_RIGHT_THUMB     10	
#define AE_CONTROLLER_BUTTON_DPAD_UP         11
#define AE_CONTROLLER_BUTTON_DPAD_RIGHT      12
#define AE_CONTROLLER_BUTTON_DPAD_DOWN       13
#define AE_CONTROLLER_BUTTON_DPAD_LEFT       14
#define AE_CONTROLLER_BUTTON_LAST            AE_CONTROLLER_BUTTON_DPAD_LEFT

	enum class ControllerButton
	{
		BUTTON_ONE = AE_CONTROLLER_BUTTON_A,
		BUTTON_TWO = AE_CONTROLLER_BUTTON_B,
		BUTTON_THREE = AE_CONTROLLER_BUTTON_X,
		BUTTON_FOUR = AE_CONTROLLER_BUTTON_Y,
		LEFT_BUMPER = AE_CONTROLLER_BUTTON_LEFT_BUMPER,
		RIGHT_BUMPER = AE_CONTROLLER_BUTTON_RIGHT_BUMPER,
		BACK = AE_CONTROLLER_BUTTON_BACK,
		START = AE_CONTROLLER_BUTTON_START,
		GUIDE = AE_CONTROLLER_BUTTON_GUIDE,
		LEFT_THUMB = AE_CONTROLLER_BUTTON_LEFT_THUMB,
		RIGHT_THUMB = AE_CONTROLLER_BUTTON_RIGHT_THUMB,
		DPAD_UP = AE_CONTROLLER_BUTTON_DPAD_UP,
		DPAD_RIGHT = AE_CONTROLLER_BUTTON_DPAD_RIGHT,
		DPAD_DOWN = AE_CONTROLLER_BUTTON_DPAD_DOWN,
		DPAD_LEFT = AE_CONTROLLER_BUTTON_DPAD_LEFT
	};

#define AE_CONTROLLER_AXIS_LEFT_X        0
#define AE_CONTROLLER_AXIS_LEFT_Y        1
#define AE_CONTROLLER_AXIS_RIGHT_X       2
#define AE_CONTROLLER_AXIS_RIGHT_Y       3
#define AE_CONTROLLER_AXIS_LAST          AE_CONTROLLER_AXIS_RIGHT_Y

	enum class ControllerAxis
	{
		LEFT_X = AE_CONTROLLER_AXIS_LEFT_X,
		LEFT_Y = AE_CONTROLLER_AXIS_LEFT_Y,
		RIGHT_X = AE_CONTROLLER_AXIS_RIGHT_X,
		RIGHT_Y = AE_CONTROLLER_AXIS_RIGHT_Y
	};

	class Controller
	{
	public:
		Controller(uint32_t id);
		~Controller();

		bool IsButtonPressed(int32_t button) const;
		bool IsButtonPressed(ControllerButton button) const;

		float GetAxis(int32_t axis) const;
		float GetAxis(ControllerAxis axis) const;

		std::string GetName() const;

		static bool IsConnected(uint32_t id);
	private:
		uint32_t m_Id;
	};

	class IconSetContainer
	{
	public:
		IconSetContainer();
		IconSetContainer(const IconSetContainer& icon) = delete;
		IconSetContainer& operator=(const IconSetContainer& icon) = delete;
		~IconSetContainer();

		inline const GLFWimage* GetImages() const
		{
			if (m_Images.empty())
			{
				return nullptr;
			}

			return m_Images.data(); 
		}

		inline int32_t GetCount() const { return static_cast<uint32_t>(m_Images.size()); }

		void AddImage(uint8_t* pPixels, int32_t width, int32_t height);
	private:
		std::vector<GLFWimage> m_Images;
		std::vector<uint8_t> m_Pixels;
	};

	class IconSet
	{
	public:
		IconSet(const std::initializer_list<std::string>& paths);
		IconSet(const IconSet& icon);
		~IconSet();

		IconSet& operator=(const IconSet& icon);

		inline const GLFWimage* GetImages() const { return m_pContainer->GetImages(); }
		inline int32_t GetCount() const { return m_pContainer->GetCount(); };
	private:
		std::shared_ptr<IconSetContainer> m_pContainer;
	};

	class CursorContainer
	{
	public:
		CursorContainer(GLFWcursor* pCursor);
		CursorContainer(const CursorContainer& cursor) = delete;
		CursorContainer& operator=(const CursorContainer& cursor) = delete;
		~CursorContainer();

		inline GLFWcursor* GetCursor() const { return m_pCursor; }
	private:
		GLFWcursor* m_pCursor;
	};

	class Cursor
	{
	public:
		Cursor(const std::string& path);
		Cursor(const std::string& path, int32_t xHot, int32_t yHot);
		Cursor(const Cursor& cursor);
		~Cursor();

		Cursor& operator=(const Cursor& cursor);

		inline GLFWcursor* GetCursor() const { return m_pContainer->GetCursor(); }
	private:
		std::shared_ptr<CursorContainer> m_pContainer;
		int32_t m_XHot;
		int32_t m_YHot;
	};

	class Window;

	class Context
	{
	public:
		Context(Window& window);
		Context(const Context&) = delete;
		Context& operator=(const Context&) = delete;
		virtual ~Context();

		inline const std::string& GetGraphicsAPI() const { return m_GraphicsAPI; }
		inline const std::string& GetGraphicsVersion() const { return m_GraphicsVersion; }
		inline const std::string& GetGraphicsCard() const { return m_GraphicsCard; }
		inline const std::string& GetGraphicsVendor() const { return m_GraphicsVendor; }
	protected:
		void Create();
		void Activate();
		void Deactivate();
		void Destroy();

		virtual bool CreateImpl() = 0;
		virtual void ActivateImpl() = 0;
		virtual void DeactivateImpl() = 0;
		virtual void DestroyImpl() = 0;
	protected:
		Window& m_Window;
		std::string m_GraphicsAPI;
		std::string m_GraphicsVersion;
		std::string m_GraphicsCard;
		std::string m_GraphicsVendor;
	private:
		bool m_Created;

		friend class Window;
	};

	class WindowManager;
	class Interface;

	class Window
	{
	public:
		Window(const WindowDesc& desc);
		Window(const WindowDesc& desc, Window& parent);
		Window(const Window&) = delete;
		Window& operator=(const Window&) = delete;
		~Window();

		void Create();
		void Destroy();

		void Clear() const;
		void Update();

		void Close();
		bool ShouldClose() const;

		inline const WindowDesc& GetDesc() { return m_Desc; }

		inline GLFWwindow* GetWindow() const { return m_pWindow; }

		inline std::weak_ptr<Context> GetContext() const { return m_pContext; }

		inline Keyboard& GetKeyboard() { return m_Keyboard; }
		inline Mouse& GetMouse() { return m_Mouse; }
		inline Controller& GetController(uint32_t index) { return m_Controllers[index]; }

		inline bool IsFocused() const { return m_Focused; }

		inline uint32_t GetWidth() const { return m_Desc.width; }
		inline uint32_t GetHeight() const { return m_Desc.height; }

		inline double GetFrameTime() const { return m_FrameTime; }
		inline double GetFrameDuration() const { return m_FrameDuration; }
		inline double GetAverageFrameTime() const { return m_AverageFrameTime; }
		inline double GetAverageFrameDuration() const { return m_AverageFrameDuration; }
		inline double GetFps() const { return m_Fps; }

		void SetTitle(const std::string& title);

		void SetIconSet(const IconSet& iconSet);
		void ResetIconSet();

		void SetCursor(const Cursor& cursor);
		void ResetCursor();

		inline bool IsActive() const { return m_Active; }
		void SetActive();

		inline const float* GetClearColor() const { return m_ClearColor; }
		inline void SetClearColor(float r, float g, float b, float a) { m_ClearColor[0] = r; m_ClearColor[1] = g; m_ClearColor[2] = b; m_ClearColor[3] = a; }
		inline void SetClearColor(float color[4]) { m_ClearColor[0] = color[0]; m_ClearColor[1] = color[1]; m_ClearColor[2] = color[2]; m_ClearColor[3] = color[3]; }

		inline void SetOnKeyPressedCB(const std::function<void(char)>& cb) { m_OnKeyPressed = cb; }
		inline void SetOnKeyReleasedCB(const std::function<void(char)>& cb) { m_OnKeyReleased = cb; }
		inline void SetOnKeyTypedCB(const std::function<void(char)>& cb) { m_OnKeyTyped = cb; }

		inline void SetOnMouseButtonPressedCB(const std::function<void(int32_t)>& cb) { m_OnMouseButtonPressed = cb; }
		inline void SetOnMouseButtonReleasedCB(const std::function<void(int32_t)>& cb) { m_OnMouseButtonReleased = cb; }
		inline void SetOnMouseMovedCB(const std::function<void(float, float)>& cb) { m_OnMouseMoved = cb; }
		inline void SetOnMouseScrolledCB(const std::function<void(float, float)>& cb) { m_OnMouseScrolled = cb; }
		inline void SetOnMouseEnteredCB(const std::function<void()>& cb) { m_OnMouseEntered = cb; }
		inline void SetOnMouseExitedCB(const std::function<void()>& cb) { m_OnMouseExited = cb; }

		inline void SetOnWindowResizeCB(const std::function<void(uint32_t, uint32_t)>& cb) { m_OnWindowResize = cb; }
		inline void SetOnWindowMinimalizedCB(const std::function<void()>& cb) { m_OnWindowMinimalized = cb; }
		inline void SetOnWindowMaximalizedCB(const std::function<void()>& cb) { m_OnWindowMaximalized = cb; }
		inline void SetOnWindowRestoredCB(const std::function<void()>& cb) { m_OnWindowRestored = cb; }
		inline void SetOnWindowMovedCB(const std::function<void(uint32_t, uint32_t)>& cb) { m_OnWindowMoved = cb; }
		inline void SetOnWindowFocusedCB(const std::function<void(bool)>& cb) { m_OnWindowFocused = cb; }

		void SetOnInterfaceUpdateCB(const std::function<void()>& cb);

	private:
		void AddChild(Window& child);
		void RemoveChild(Window& child);
		
		GLFWmonitor* GetMonitor();

		void CreateWindowed();
		void CreateFullscreen();

		void InitOpenGL();
#ifdef AE_VULKAN
		void InitVulkan();
#endif // AE_VULKAN

		void CreateOpenGL();
#ifdef AE_VULKAN
		void CreateVulkan();
#endif // AE_VULKAN

		void UpdateOpenGL();
#ifdef AE_VULKAN
		void UpdateVulkan();
#endif // AE_VULKAN

		void DestroyOpenGL();
#ifdef AE_VULKAN
		void DestroyVulkan();
#endif // AE_VULKAN

		void InitInput();

		void OnKey(int key, int scancode, int action, int mods);
		void OnChar(unsigned int c);

		void OnMouseButton(int button, int action, int mods);
		void OnMouseMoved(double x, double y);
		void OnMouseScrolled(double x, double y);
		void OnMouseEntered(int entered);

		void OnWindowResize(uint32_t width, uint32_t height);
		void OnWindowMinimalized();
		void OnWindowMaximalized();
		void OnWindowRestored();
		void OnWindowMoved(uint32_t x, uint32_t y);
		void OnWindowFocused(int focused);

		void OnMonitor(GLFWmonitor* pMonitor, int event);

		void Deactivate();
	private:
		WindowDesc m_Desc;
		Window* m_pParent;
		std::vector<Window*> m_Children;
		GLFWwindow* m_pWindow;
		ae::Timer m_Timer;
		ae::Timer m_FrameTimer;

		double m_FrameTime;
		double m_FrameDuration;
		double m_AverageFrameTime;
		double m_AverageFrameDuration;
		double m_Fps;

		Keyboard m_Keyboard;
		Mouse m_Mouse;
		std::vector<Controller> m_Controllers;

		std::shared_ptr<Context> m_pContext;
		float m_ClearColor[4];
		std::unique_ptr<Interface> m_pInterface;

		std::function<void(char)> m_OnKeyPressed = nullptr;
		std::function<void(char)> m_OnKeyReleased = nullptr;
		std::function<void(char)> m_OnKeyTyped = nullptr;

		std::function<void(int32_t)> m_OnMouseButtonPressed = nullptr;
		std::function<void(int32_t)> m_OnMouseButtonReleased = nullptr;
		std::function<void(float, float)> m_OnMouseMoved = nullptr;
		std::function<void(float, float)> m_OnMouseScrolled = nullptr;
		std::function<void()> m_OnMouseEntered = nullptr;
		std::function<void()> m_OnMouseExited = nullptr;

		std::function<void(uint32_t, uint32_t)> m_OnWindowResize = nullptr;
		std::function<void()> m_OnWindowMinimalized = nullptr;
		std::function<void()> m_OnWindowMaximalized = nullptr;
		std::function<void()> m_OnWindowRestored = nullptr;
		std::function<void(uint32_t, uint32_t)> m_OnWindowMoved = nullptr;
		std::function<void(bool)> m_OnWindowFocused = nullptr;

		std::function<void()> m_OnMonitorConnected = nullptr;

		bool m_Focused;
		bool m_Active;
		bool m_Created;

		friend class WindowManager;
	};
}
