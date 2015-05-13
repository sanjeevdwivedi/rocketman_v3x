/****************************************************************************
Copyright (c) 2013 cocos2d-x.org
Copyright (c) Microsoft Open Technologies, Inc.

http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/

#include "CCGLViewImpl-winrt.h"
#include "base/ccMacros.h"
#include "base/CCDirector.h"
#include "base/CCTouch.h"
#include "base/CCIMEDispatcher.h"
#include "CCApplication.h"
#include "CCWinRTUtils.h"
#include "deprecated/CCNotificationCenter.h"
#include <map>

using namespace Platform;
using namespace Concurrency;
using namespace Windows::System;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Graphics::Display;
using namespace Windows::UI::Input;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::System;
using namespace Windows::UI::ViewManagement;
using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::ApplicationModel::Activation;
using namespace Platform;
using namespace Microsoft::WRL;


NS_CC_BEGIN

static GLViewImpl* s_pEglView = NULL;

struct keyCodeItem
{
	int key;
	EventKeyboard::KeyCode keyCode;
};

static std::map<int, EventKeyboard::KeyCode> g_keyCodeMap;

// http://www.kbdedit.com/manual/low_level_vk_list.html
// https://msdn.microsoft.com/library/windows/apps/windows.system.virtualkey.aspx

static keyCodeItem g_keyCodeStructArray[] = {
	/* The unknown key */
	{ (int)VirtualKey::None			, EventKeyboard::KeyCode::KEY_NONE },
	/* Printable keys */
	{ (int)VirtualKey::Space		, EventKeyboard::KeyCode::KEY_SPACE },
	{ (int)VK_OEM_7					, EventKeyboard::KeyCode::KEY_APOSTROPHE },
	{ (int)VK_OEM_COMMA				, EventKeyboard::KeyCode::KEY_COMMA },
	{ (int)VK_OEM_MINUS				, EventKeyboard::KeyCode::KEY_MINUS },
	{ (int)VK_OEM_PERIOD			, EventKeyboard::KeyCode::KEY_PERIOD },
	{ (int)VK_OEM_2					, EventKeyboard::KeyCode::KEY_SLASH },
	{ (int)VK_OEM_3					, EventKeyboard::KeyCode::KEY_TILDE },

	{ (int)VirtualKey::Number0		, EventKeyboard::KeyCode::KEY_0 },
	{ (int)VirtualKey::Number1		, EventKeyboard::KeyCode::KEY_1 },
	{ (int)VirtualKey::Number2		, EventKeyboard::KeyCode::KEY_2 },
	{ (int)VirtualKey::Number3		, EventKeyboard::KeyCode::KEY_3 },
	{ (int)VirtualKey::Number4		, EventKeyboard::KeyCode::KEY_4 },
	{ (int)VirtualKey::Number5		, EventKeyboard::KeyCode::KEY_5 },
	{ (int)VirtualKey::Number6		, EventKeyboard::KeyCode::KEY_6 },
	{ (int)VirtualKey::Number7		, EventKeyboard::KeyCode::KEY_7 },
	{ (int)VirtualKey::Number8		, EventKeyboard::KeyCode::KEY_8 },
	{ (int)VirtualKey::Number9		, EventKeyboard::KeyCode::KEY_9 },
	{ (int)VK_OEM_1  				, EventKeyboard::KeyCode::KEY_SEMICOLON },
	{ (int)VK_OEM_PLUS				, EventKeyboard::KeyCode::KEY_EQUAL },
	{ (int)VirtualKey::A			, EventKeyboard::KeyCode::KEY_A },
	{ (int)VirtualKey::B			, EventKeyboard::KeyCode::KEY_B },
	{ (int)VirtualKey::C			, EventKeyboard::KeyCode::KEY_C },
	{ (int)VirtualKey::D			, EventKeyboard::KeyCode::KEY_D },
	{ (int)VirtualKey::E			, EventKeyboard::KeyCode::KEY_E },
	{ (int)VirtualKey::F			, EventKeyboard::KeyCode::KEY_F },
	{ (int)VirtualKey::G			, EventKeyboard::KeyCode::KEY_G },
	{ (int)VirtualKey::H			, EventKeyboard::KeyCode::KEY_H },
	{ (int)VirtualKey::I			, EventKeyboard::KeyCode::KEY_I },
	{ (int)VirtualKey::J			, EventKeyboard::KeyCode::KEY_J },
	{ (int)VirtualKey::K			, EventKeyboard::KeyCode::KEY_K },
	{ (int)VirtualKey::L			, EventKeyboard::KeyCode::KEY_L },
	{ (int)VirtualKey::M			, EventKeyboard::KeyCode::KEY_M },
	{ (int)VirtualKey::N			, EventKeyboard::KeyCode::KEY_N },
	{ (int)VirtualKey::O			, EventKeyboard::KeyCode::KEY_O },
	{ (int)VirtualKey::P			, EventKeyboard::KeyCode::KEY_P },
	{ (int)VirtualKey::Q			, EventKeyboard::KeyCode::KEY_Q },
	{ (int)VirtualKey::R			, EventKeyboard::KeyCode::KEY_R },
	{ (int)VirtualKey::S			, EventKeyboard::KeyCode::KEY_S },
	{ (int)VirtualKey::T			, EventKeyboard::KeyCode::KEY_T },
	{ (int)VirtualKey::U			, EventKeyboard::KeyCode::KEY_U },
	{ (int)VirtualKey::V			, EventKeyboard::KeyCode::KEY_V },
	{ (int)VirtualKey::W			, EventKeyboard::KeyCode::KEY_W },
	{ (int)VirtualKey::X			, EventKeyboard::KeyCode::KEY_X },
	{ (int)VirtualKey::Y			, EventKeyboard::KeyCode::KEY_Y },
	{ (int)VirtualKey::Z			, EventKeyboard::KeyCode::KEY_Z },
	{ VK_OEM_4						, EventKeyboard::KeyCode::KEY_LEFT_BRACKET },
	{ VK_OEM_5						, EventKeyboard::KeyCode::KEY_BACK_SLASH },
	{ VK_OEM_6						, EventKeyboard::KeyCode::KEY_RIGHT_BRACKET },
//	{ GLFW_KEY_GRAVE_ACCENT			, EventKeyboard::KeyCode::KEY_GRAVE },

	/* Function keys */
	{ (int)VirtualKey::Escape		, EventKeyboard::KeyCode::KEY_ESCAPE },
	{ (int)VirtualKey::Enter		, EventKeyboard::KeyCode::KEY_ENTER },
	{ (int)VirtualKey::Tab			, EventKeyboard::KeyCode::KEY_TAB },
	{ (int)VirtualKey::Back			, EventKeyboard::KeyCode::KEY_BACKSPACE },
	{ (int)VirtualKey::Insert		, EventKeyboard::KeyCode::KEY_INSERT },
	{ (int)VirtualKey::Delete		, EventKeyboard::KeyCode::KEY_DELETE },
	{ (int)VirtualKey::Right		, EventKeyboard::KeyCode::KEY_RIGHT_ARROW },
	{ (int)VirtualKey::Left			, EventKeyboard::KeyCode::KEY_LEFT_ARROW },
	{ (int)VirtualKey::Down			, EventKeyboard::KeyCode::KEY_DOWN_ARROW },
	{ (int)VirtualKey::Up			, EventKeyboard::KeyCode::KEY_UP_ARROW },
	{ VK_PRIOR						, EventKeyboard::KeyCode::KEY_KP_PG_UP },
	{ VK_NEXT						, EventKeyboard::KeyCode::KEY_KP_PG_DOWN },
	{ VK_HOME						, EventKeyboard::KeyCode::KEY_KP_HOME },
	{ VK_END						, EventKeyboard::KeyCode::KEY_END },
	{ VK_CAPITAL					, EventKeyboard::KeyCode::KEY_CAPS_LOCK },
	{ VK_SCROLL						, EventKeyboard::KeyCode::KEY_SCROLL_LOCK },
	{ VK_NUMLOCK					, EventKeyboard::KeyCode::KEY_NUM_LOCK },
	{ VK_SNAPSHOT					, EventKeyboard::KeyCode::KEY_PRINT },
	{ VK_PAUSE						, EventKeyboard::KeyCode::KEY_PAUSE },
	{ (int)VirtualKey::F1			, EventKeyboard::KeyCode::KEY_F1 },
	{ (int)VirtualKey::F2			, EventKeyboard::KeyCode::KEY_F2 },
	{ (int)VirtualKey::F3			, EventKeyboard::KeyCode::KEY_F3 },
	{ (int)VirtualKey::F4			, EventKeyboard::KeyCode::KEY_F4 },
	{ (int)VirtualKey::F5			, EventKeyboard::KeyCode::KEY_F5 },
	{ (int)VirtualKey::F6			, EventKeyboard::KeyCode::KEY_F6 },
	{ (int)VirtualKey::F7			, EventKeyboard::KeyCode::KEY_F7 },
	{ (int)VirtualKey::F8			, EventKeyboard::KeyCode::KEY_F8 },
	{ (int)VirtualKey::F9			, EventKeyboard::KeyCode::KEY_F9 },
	{ (int)VirtualKey::F10			, EventKeyboard::KeyCode::KEY_F10 },
	{ (int)VirtualKey::F11			, EventKeyboard::KeyCode::KEY_F11 },
	{ (int)VirtualKey::F12			, EventKeyboard::KeyCode::KEY_F12 },
	{ (int)VirtualKey::F13			, EventKeyboard::KeyCode::KEY_NONE },
	{ (int)VirtualKey::F14			, EventKeyboard::KeyCode::KEY_NONE },
	{ (int)VirtualKey::F15			, EventKeyboard::KeyCode::KEY_NONE },
	{ (int)VirtualKey::F16			, EventKeyboard::KeyCode::KEY_NONE },
	{ (int)VirtualKey::F17			, EventKeyboard::KeyCode::KEY_NONE },
	{ (int)VirtualKey::F18			, EventKeyboard::KeyCode::KEY_NONE },
	{ (int)VirtualKey::F19			, EventKeyboard::KeyCode::KEY_NONE },
	{ (int)VirtualKey::F20			, EventKeyboard::KeyCode::KEY_NONE },
	{ (int)VirtualKey::F21			, EventKeyboard::KeyCode::KEY_NONE },
	{ (int)VirtualKey::F22			, EventKeyboard::KeyCode::KEY_NONE },
	{ (int)VirtualKey::F23			, EventKeyboard::KeyCode::KEY_NONE },
	{ (int)VirtualKey::F24			, EventKeyboard::KeyCode::KEY_NONE },

	{ (int)VirtualKey::NumberPad0	, EventKeyboard::KeyCode::KEY_0 },
	{ (int)VirtualKey::NumberPad1  	, EventKeyboard::KeyCode::KEY_1 },
	{ (int)VirtualKey::NumberPad2  	, EventKeyboard::KeyCode::KEY_2 },
	{ (int)VirtualKey::NumberPad3  	, EventKeyboard::KeyCode::KEY_3 },
	{ (int)VirtualKey::NumberPad4  	, EventKeyboard::KeyCode::KEY_4 },
	{ (int)VirtualKey::NumberPad5  	, EventKeyboard::KeyCode::KEY_5 },
	{ (int)VirtualKey::NumberPad6  	, EventKeyboard::KeyCode::KEY_6 },
	{ (int)VirtualKey::NumberPad7  	, EventKeyboard::KeyCode::KEY_7 },
	{ (int)VirtualKey::NumberPad8  	, EventKeyboard::KeyCode::KEY_8 },
	{ (int)VirtualKey::NumberPad9  	, EventKeyboard::KeyCode::KEY_9 },
#if 0
	{ GLFW_KEY_KP_1					, EventKeyboard::KeyCode::KEY_1 },
	{ GLFW_KEY_KP_2					, EventKeyboard::KeyCode::KEY_2 },
	{ GLFW_KEY_KP_3					, EventKeyboard::KeyCode::KEY_3 },
	{ GLFW_KEY_KP_4					, EventKeyboard::KeyCode::KEY_4 },
	{ GLFW_KEY_KP_5					, EventKeyboard::KeyCode::KEY_5 },
	{ GLFW_KEY_KP_6					, EventKeyboard::KeyCode::KEY_6 },
	{ GLFW_KEY_KP_7					, EventKeyboard::KeyCode::KEY_7 },
	{ GLFW_KEY_KP_8					, EventKeyboard::KeyCode::KEY_8 },
	{ GLFW_KEY_KP_9					, EventKeyboard::KeyCode::KEY_9 },
#endif
	{ (int)VirtualKey::Decimal		, EventKeyboard::KeyCode::KEY_PERIOD },
	{ (int)VirtualKey::Divide		, EventKeyboard::KeyCode::KEY_KP_DIVIDE },
	{ (int)VirtualKey::Multiply		, EventKeyboard::KeyCode::KEY_KP_MULTIPLY },
	{ (int)VirtualKey::Subtract		, EventKeyboard::KeyCode::KEY_KP_MINUS },
	{ (int)VirtualKey::Add			, EventKeyboard::KeyCode::KEY_KP_PLUS },
	//{ GLFW_KEY_KP_ENTER        , EventKeyboard::KeyCode::KEY_KP_ENTER },
	//{ GLFW_KEY_KP_EQUAL        , EventKeyboard::KeyCode::KEY_EQUAL },
	{ (int)VirtualKey::Shift		, EventKeyboard::KeyCode::KEY_LEFT_SHIFT },
	{ (int)VirtualKey::Control		, EventKeyboard::KeyCode::KEY_LEFT_CTRL },
	{ VK_LMENU						, EventKeyboard::KeyCode::KEY_LEFT_ALT },
	{ (int)VirtualKey::LeftWindows	, EventKeyboard::KeyCode::KEY_HYPER },
	{ (int)VirtualKey::RightShift	, EventKeyboard::KeyCode::KEY_RIGHT_SHIFT },
	{ (int)VirtualKey::RightControl	, EventKeyboard::KeyCode::KEY_RIGHT_CTRL },
	{ VK_RMENU						, EventKeyboard::KeyCode::KEY_RIGHT_ALT },
	{ (int)VirtualKey::RightWindows	, EventKeyboard::KeyCode::KEY_HYPER },
	{ (int)VirtualKey::Menu			, EventKeyboard::KeyCode::KEY_MENU },
	{ (int)VirtualKey::LeftMenu		, EventKeyboard::KeyCode::KEY_MENU },
	{ (int)VirtualKey::RightMenu	, EventKeyboard::KeyCode::KEY_MENU }
};

GLViewImpl* GLViewImpl::create(const std::string& viewName)
{
    auto ret = new GLViewImpl;
    if(ret && ret->initWithFullScreen(viewName))
    {
        ret->autorelease();
        return ret;
    }

    return nullptr;
}

GLViewImpl::GLViewImpl()
	: _frameZoomFactor(1.0f)
	, _supportTouch(true)
    , _isRetina(false)
	, m_lastPointValid(false)
	, m_running(false)
	, m_initialized(false)
	, m_windowClosed(false)
	, m_windowVisible(true)
    , m_width(0)
    , m_height(0)
    , m_orientation(DisplayOrientations::Landscape)
    , m_appShouldExit(false)
{
	s_pEglView = this;
    _viewName =  "cocos2dx";

    m_backButtonListener = EventListenerKeyboard::create();
    m_backButtonListener->onKeyReleased = CC_CALLBACK_2(GLViewImpl::BackButtonListener, this);
    Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(m_backButtonListener, INT_MAX);

	g_keyCodeMap.clear();
	for (auto& item : g_keyCodeStructArray)
	{
		g_keyCodeMap[item.key] = item.keyCode;
	}
}

GLViewImpl::~GLViewImpl()
{
	CC_ASSERT(this == s_pEglView);
    s_pEglView = NULL;

	// TODO: cleanup 
}

bool GLViewImpl::initWithRect(const std::string& viewName, Rect rect, float frameZoomFactor)
{
    setViewName(viewName);
    setFrameSize(rect.size.width, rect.size.height);
    setFrameZoomFactor(frameZoomFactor);
    return true;
}

bool GLViewImpl::initWithFullScreen(const std::string& viewName)
{
    return initWithRect(viewName, Rect(0, 0, m_width, m_height), 1.0f);
}


bool GLViewImpl::Create(float width, float height, float dpi, DisplayOrientations orientation)
{
    m_orientation = orientation;
    m_dpi = dpi;
    UpdateForWindowSizeChange(width, height);
    return true;
}

void GLViewImpl::setDispatcher(Windows::UI::Core::CoreDispatcher^ dispatcher)
{
    m_dispatcher = dispatcher;
}

void GLViewImpl::setPanel(Windows::UI::Xaml::Controls::Panel^ panel)
{
    m_panel = panel;
}

void GLViewImpl::setIMEKeyboardState(bool bOpen)
{
    std::string str;
    setIMEKeyboardState(bOpen, str);
}

bool GLViewImpl::ShowMessageBox(Platform::String^ title, Platform::String^ message)
{
    if (m_dispatcher.Get())
    {
        m_dispatcher.Get()->RunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal, ref new Windows::UI::Core::DispatchedHandler([title, message]()
        {
            // Show the message dialog
            auto msg = ref new Windows::UI::Popups::MessageDialog(message, title);
            // Set the command to be invoked when a user presses 'ESC'
            msg->CancelCommandIndex = 1;
            msg->ShowAsync();
        }));

        return true;
    }
    return false;
}

void GLViewImpl::setIMEKeyboardState(bool bOpen, std::string str)
{
    if(bOpen)
    {
        if (m_keyboard == nullptr)
        {
            m_keyboard = ref new KeyBoardWinRT(m_dispatcher.Get(), m_panel.Get());
        }
        m_keyboard->ShowKeyboard(PlatformStringFromString(str));
    }
    else
    {
        if (m_keyboard != nullptr)
        {
            m_keyboard->HideKeyboard(PlatformStringFromString(str));
        }
        m_keyboard = nullptr;
    }
}

void GLViewImpl::swapBuffers()
{
    
}

bool GLViewImpl::isOpenGLReady()
{
    return true;
}

void GLViewImpl::end()
{
	m_windowClosed = true;
    m_appShouldExit = true;
}

void GLViewImpl::OnSuspending(Platform::Object^ sender, SuspendingEventArgs^ args)
{
}

void GLViewImpl::OnResuming(Platform::Object^ sender, Platform::Object^ args)
{
}

// user pressed the Back Key on the phone
void GLViewImpl::OnBackKeyPress()
{
    cocos2d::EventKeyboard::KeyCode cocos2dKey = EventKeyboard::KeyCode::KEY_ESCAPE;
    cocos2d::EventKeyboard event(cocos2dKey, false);
    cocos2d::Director::getInstance()->getEventDispatcher()->dispatchEvent(&event);
}

void GLViewImpl::BackButtonListener(EventKeyboard::KeyCode keyCode, Event* event)
{
	if (keyCode == EventKeyboard::KeyCode::KEY_ESCAPE)
	{
		CCLOG("*********************************************************************");
		CCLOG("GLViewImpl::BackButtonListener: Exiting application!");
		CCLOG("");
		CCLOG("If you want to listen for Windows Phone back button events,");
		CCLOG("add a listener for EventKeyboard::KeyCode::KEY_ESCAPE");
		CCLOG("Make sure you call stopPropagation() on the Event if you don't");
		CCLOG("want your app to exit when the back button is pressed.");
		CCLOG("");
		CCLOG("For example, add the following to your scene...");
		CCLOG("auto listener = EventListenerKeyboard::create();");
		CCLOG("listener->onKeyReleased = CC_CALLBACK_2(HelloWorld::onKeyReleased, this);");
		CCLOG("getEventDispatcher()->addEventListenerWithFixedPriority(listener, 1);");
		CCLOG("");
		CCLOG("void HelloWorld::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event)");
		CCLOG("{");
		CCLOG("     if (keyCode == EventKeyboard::KeyCode::KEY_ESCAPE)");
		CCLOG("     {");
		CCLOG("         if (myAppShouldNotQuit) // or whatever logic you want...");
		CCLOG("         {");
		CCLOG("             event->stopPropagation();");
		CCLOG("         }");
		CCLOG("     }");
		CCLOG("}");
		CCLOG("");
		CCLOG("You MUST call event->stopPropagation() if you don't want your app to quit!");
		CCLOG("*********************************************************************");

		Director::getInstance()->end();
    }
}

bool GLViewImpl::AppShouldExit()
{
    return m_appShouldExit;
}

void GLViewImpl::OnPointerPressed(CoreWindow^ sender, PointerEventArgs^ args)
{
    OnPointerPressed(args);
}

void GLViewImpl::OnPointerPressed(PointerEventArgs^ args)
{
    int id = args->CurrentPoint->PointerId;
    Vec2 pt = GetPoint(args);
    handleTouchesBegin(1, &id, &pt.x, &pt.y);
}

void GLViewImpl::OnPointerWheelChanged(CoreWindow^ sender, PointerEventArgs^ args)
{
    float direction = (float)args->CurrentPoint->Properties->MouseWheelDelta;
    int id = 0;
    Vec2 p(0.0f,0.0f);
    handleTouchesBegin(1, &id, &p.x, &p.y);
    p.y += direction;
    handleTouchesMove(1, &id, &p.x, &p.y);
    handleTouchesEnd(1, &id, &p.x, &p.y);
}

void GLViewImpl::OnVisibilityChanged(CoreWindow^ sender, VisibilityChangedEventArgs^ args)
{
	m_windowVisible = args->Visible;
}

void GLViewImpl::OnWindowClosed(CoreWindow^ sender, CoreWindowEventArgs^ args)
{
	m_windowClosed = true;
}

void GLViewImpl::OnPointerMoved(CoreWindow^ sender, PointerEventArgs^ args)
{
    OnPointerMoved(args);   
}

void GLViewImpl::OnPointerMoved( PointerEventArgs^ args)
{
	auto currentPoint = args->CurrentPoint;
	if (currentPoint->IsInContact)
	{
		if (m_lastPointValid)
		{
			int id = args->CurrentPoint->PointerId;
			Vec2 p = GetPoint(args);
			handleTouchesMove(1, &id, &p.x, &p.y);
		}
		m_lastPoint = currentPoint->Position;
		m_lastPointValid = true;
	}
	else
	{
		m_lastPointValid = false;
	}
}

void GLViewImpl::OnPointerReleased(CoreWindow^ sender, PointerEventArgs^ args)
{
    OnPointerReleased(args);
}

void GLViewImpl::OnPointerReleased(PointerEventArgs^ args)
{
    int id = args->CurrentPoint->PointerId;
    Vec2 pt = GetPoint(args);
    handleTouchesEnd(1, &id, &pt.x, &pt.y);
}



void GLViewImpl::resize(int width, int height)
{

}

void GLViewImpl::setFrameZoomFactor(float fZoomFactor)
{
    _frameZoomFactor = fZoomFactor;
    Director::getInstance()->setProjection(Director::getInstance()->getProjection());
    //resize(m_obScreenSize.width * fZoomFactor, m_obScreenSize.height * fZoomFactor);
}

float GLViewImpl::getFrameZoomFactor()
{
    return _frameZoomFactor;
}

void GLViewImpl::centerWindow()
{
	// not implemented in WinRT. Window is always full screen
}

GLViewImpl* GLViewImpl::sharedOpenGLView()
{
    return s_pEglView;
}

int GLViewImpl::Run() 
{
    // XAML version does not have a run loop
	m_running = true; 
    return 0;
};

void GLViewImpl::Render()
{
    OnRendering();
}

void GLViewImpl::OnRendering()
{
	if(m_running && m_initialized)
	{
        Director::getInstance()->mainLoop();
	}
}

// called by orientation change from WP8 XAML
void GLViewImpl::UpdateOrientation(DisplayOrientations orientation)
{
    if(m_orientation != orientation)
    {
        m_orientation = orientation;
        UpdateWindowSize();
    }
}

// called by size change from WP8 XAML
void GLViewImpl::UpdateForWindowSizeChange(float width, float height)
{
    if (width != m_width || height != m_height)
    {
        m_width = width;
        m_height = height;
        UpdateWindowSize();
    }
}

#if 0
win32 version

void GLViewEventHandler::OnGLFWWindowSizeFunCallback(GLFWwindow *windows, int width, int height)
{	
	auto view = Director::getInstance()->getOpenGLView();
	if(view && view->getResolutionPolicy() != ResolutionPolicy::UNKNOWN)
	{
		Size resSize=view->getDesignResolutionSize();
		ResolutionPolicy resPolicy=view->getResolutionPolicy();
		view->setFrameSize(width, height);
 		view->setDesignResolutionSize(resSize.width, resSize.height, resPolicy);
		Director::getInstance()->setViewport();
	}
}
#endif

void GLViewImpl::UpdateWindowSize()
{
    float width, height;

    width = m_width;
    height = m_height;


    //CCSize designSize = getDesignResolutionSize();
    if(!m_initialized)
    {
        m_initialized = true;
        GLView::setFrameSize(width, height);
    }

    auto view = Director::getInstance()->getOpenGLView();
	if(view && view->getResolutionPolicy() != ResolutionPolicy::UNKNOWN)
	{
		Size resSize=view->getDesignResolutionSize();
		ResolutionPolicy resPolicy=view->getResolutionPolicy();
		view->setFrameSize(width, height);
 		view->setDesignResolutionSize(resSize.width, resSize.height, resPolicy);
        auto director = Director::getInstance();
        director->setViewport();
        director->setProjection(director->getProjection());
	}
}

cocos2d::Vec2 GLViewImpl::TransformToOrientation(Windows::Foundation::Point p)
{
    cocos2d::Vec2 returnValue;

    float x = p.X;
    float y = p.Y;  
    returnValue = Vec2(x, y);

#if 0
    switch (m_orientation)
    {
        case DisplayOrientations::Portrait:
        default:
            returnValue = Vec2(x, y);
            break;
        case DisplayOrientations::Landscape:
            returnValue = Vec2(y, m_width - x);
            break;
        case DisplayOrientations::PortraitFlipped:
            returnValue = Vec2(m_width - x, m_height - y);
            break;
        case DisplayOrientations::LandscapeFlipped:
            returnValue = Vec2(m_height - y, x);
            break;
    }
#endif

	float zoomFactor = GLViewImpl::sharedOpenGLView()->getFrameZoomFactor();
	if(zoomFactor > 0.0f) {
		returnValue.x /= zoomFactor;
		returnValue.y /= zoomFactor;
	}

    // CCLOG("%.2f %.2f : %.2f %.2f", p.X, p.Y,returnValue.x, returnValue.y);

    return returnValue;
}

Vec2 GLViewImpl::GetPoint(PointerEventArgs^ args) {

	return TransformToOrientation(args->CurrentPoint->Position);
}


void GLViewImpl::setViewPortInPoints(float x , float y , float w , float h)
{
    glViewport((GLint) (x * _scaleX + _viewPortRect.origin.x),
        (GLint) (y * _scaleY + _viewPortRect.origin.y),
        (GLsizei) (w * _scaleX),
        (GLsizei) (h * _scaleY));
}

void GLViewImpl::setScissorInPoints(float x , float y , float w , float h)
{
    glScissor((GLint) (x * _scaleX + _viewPortRect.origin.x),
        (GLint) (y * _scaleY + _viewPortRect.origin.y),
        (GLsizei) (w * _scaleX),
        (GLsizei) (h * _scaleY));
}

void GLViewImpl::QueueBackKeyPress()
{
    std::shared_ptr<BackButtonEvent> e(new BackButtonEvent());
    mInputEvents.push(e);
}

void GLViewImpl::QueuePointerEvent(PointerEventType type, PointerEventArgs^ args)
{
    std::shared_ptr<PointerEvent> e(new PointerEvent(type, args));
    mInputEvents.push(e);
}

void GLViewImpl::QueueWinRTKeyboardEvent(WinRTKeyboardEventType type, KeyEventArgs^ args)
{
	std::shared_ptr<WinRTKeyboardEvent> e(new WinRTKeyboardEvent(type, args));
	mInputEvents.push(e);
}

void GLViewImpl::OnWinRTKeyboardEvent(WinRTKeyboardEventType type, KeyEventArgs^ args)
{
	bool pressed = (type == WinRTKeyboardEventType::KeyPressed);

	// don't allow key repeats
	if (pressed && args->KeyStatus.WasKeyDown)
	{
		return;
	}

	int key = static_cast<int>(args->VirtualKey);
	auto it = g_keyCodeMap.find(key);
	if (it != g_keyCodeMap.end())
	{
		switch (it->second)
		{
			case EventKeyboard::KeyCode::KEY_BACKSPACE:
				if (pressed)
				{
					IMEDispatcher::sharedDispatcher()->dispatchDeleteBackward();
				}
				break;
			default:
				EventKeyboard event(it->second, pressed);
				auto dispatcher = Director::getInstance()->getEventDispatcher();
				dispatcher->dispatchEvent(&event);
				break;
		}
	}
	else
	{
		log("GLViewImpl::OnWinRTKeyboardEvent Virtual Key Code %d not supported", key);
	}
}

void GLViewImpl::QueueEvent(std::shared_ptr<InputEvent>& event)
{
    mInputEvents.push(event);
}

void GLViewImpl::ProcessEvents()
{
    std::shared_ptr<InputEvent> e;
    while (mInputEvents.try_pop(e))
    {
        e->execute();
    }
}

NS_CC_END
