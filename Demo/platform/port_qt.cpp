#include <QDebug>
#include <QDirIterator>
#include <QMouseEvent>
#include <QPainter>
#include <QWindow>
#include <QOpenGLContext>
#include <QOpenGLPaintDevice>
#include <QOpenGLFunctions>
#include <QApplication>

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>

#include "tb_skin.h"
#include "tb_system.h"
#include "tb_msg.h"
#include "renderers/tb_renderer_gl.h"
#include "tb_font_renderer.h"

#include "Application.h"

#if defined(TB_TARGET_MACOSX) || defined(Q_OS_MAC)
# include <mach-o/dyld.h>
#endif

using namespace tb;

int mouse_x = 0;
int mouse_y = 0;
bool key_alt = false;
bool key_ctrl = false;
bool key_shift = false;
bool key_super = false;


class AppBackendQt;
class Window;

static void ReschedulePlatformTimer(double fire_time, bool force);
static bool ShouldEmulateTouchEvent();
static MODIFIER_KEYS GetModifierKeys();
static MODIFIER_KEYS GetModifierKeys(int mod);
static bool InvokeShortcut(int key, SPECIAL_KEY special_key, MODIFIER_KEYS modifierkeys, bool down);
static bool InvokeKey(Window *window, unsigned int key, SPECIAL_KEY special_key, MODIFIER_KEYS modifierkeys, bool down);


// == Qt Window ==

#define GetBackend(w) w->backend()

class Window : public QWindow, protected QOpenGLFunctions
{
	Q_OBJECT
	typedef bool (*CallbackFunc)(Window*, AppBackendQt*);
private:
	bool m_done, m_update_pending, m_auto_refresh;
	QOpenGLContext *m_context;
	QOpenGLPaintDevice *m_device;
	CallbackFunc m_render, m_init;

	AppBackendQt *m_backend;	
public:
	QPoint cursorPos;
public:
	Window(AppBackendQt *backend = 0, QWindow *parent = 0) : QWindow(parent)
		, m_update_pending(false)
		, m_auto_refresh(true)
		, m_context(0)
		, m_device(0)
		, m_render(0), m_init(0)
		, m_done(false)
		, m_backend(backend) {
		setSurfaceType(QWindow::OpenGLSurface);
	}
	~Window() { delete m_device; }
	
	void setAutoRefresh(bool a) { m_auto_refresh = a; }
	void setCallback(CallbackFunc i, CallbackFunc r) { m_init = i; m_render = r; }
	
	void render(QPainter *painter) {
		Q_UNUSED(painter);
		if (m_render) m_render(this, m_backend);
	}
	
	void initialize() {
		qDebug() << "OpenGL infos with gl functions:";
		qDebug() << "-------------------------------";
		qDebug() << " Renderer:" << (const char*)glGetString(GL_RENDERER);
		qDebug() << " Vendor:" << (const char*)glGetString(GL_VENDOR);
		qDebug() << " OpenGL Version:" << (const char*)glGetString(GL_VERSION);
		qDebug() << " GLSL Version:" << (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION);
		
		if (m_init) m_done = !m_init(this, m_backend);
	}
	void update() { renderLater(); }
	void render() {
		if (!m_device) m_device = new QOpenGLPaintDevice;
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		m_device->setSize(size());
		QPainter painter(m_device);
		render(&painter);
	}
	void mousePressEvent(QMouseEvent *event) {
		cursorPos = QPoint(event->x(), event->y());
		Qt::KeyboardModifiers modifiers = event->modifiers();
		if (event->buttons() & Qt::LeftButton) { }
	}
	void mouseReleaseEvent(QMouseEvent *event) {
		cursorPos = QPoint(event->x(), event->y());
		Qt::KeyboardModifiers modifiers = event->modifiers();
		if (event->button() == Qt::LeftButton) { }
	}
	void mouseMoveEvent(QMouseEvent *event) {
		cursorPos = QPoint(event->x(), event->y());
	}
	void keyPressEvent(QKeyEvent* event) {
		switch(event->key()) {
		case Qt::Key_Escape: quit(); break;
		default: event->ignore();
			break;
		}
	}
	void quit() { m_done = true; }
	bool done() const { return m_done; }
	AppBackendQt *backend() { return m_backend; }
protected:
	void closeEvent(QCloseEvent *event) { quit(); }
	bool event(QEvent *event) {
		switch (event->type()) {
		case QEvent::UpdateRequest:
			m_update_pending = false;
			renderNow();
			return true;
		default:
			return QWindow::event(event);
		}
	}
	void exposeEvent(QExposeEvent *event) {
		Q_UNUSED(event);
		if (isExposed()) renderNow();
	}
	
	public slots:
	void renderLater() {
		if (!m_update_pending) {
			m_update_pending = true;
			QCoreApplication::postEvent(this, new QEvent(QEvent::UpdateRequest));
		}
	}
	void renderNow() {
		if (!isExposed()) return;
		bool needsInitialize = false;
		if (!m_context) {
			m_context = new QOpenGLContext(this);
			m_context->setFormat(requestedFormat());
			m_context->create();
			needsInitialize = true;
		}
		m_context->makeCurrent(this);
		if (needsInitialize) {
			initializeOpenGLFunctions();
			initialize();
		}
		render();
		m_context->swapBuffers(this);
		if (m_auto_refresh) renderLater();
	}
};



// == Port interface ==

// This doesn't really belong here (it belongs in tb_system_[linux/windows].cpp.
// This is here since the proper implementations has not yet been done.
void TBSystem::RescheduleTimer(double fire_time)
{
	ReschedulePlatformTimer(fire_time, false);
}


class AppBackendQt : public QWidget, public AppBackend
{
	Q_OBJECT
	AppBackendQt()	: m_app(nullptr)
						, m_renderer(nullptr)
						, m_mainWindow(0)
						, m_plat_timer(-1)
						, m_has_pending_update(false)
						, m_quit_requested(false) {}
	~AppBackendQt() {}
public:
	static AppBackendQt *Shared() {
		static AppBackendQt shared;
		return &shared;
	}
	bool Init(App *app);
	void Close();

	virtual void OnAppEvent(const EVENT &ev);

	TBWidget *GetRoot() const { return m_app->GetRoot(); }
	int GetWidth() const { return m_app->GetWidth(); }
	int GetHeight() const { return m_app->GetHeight(); }

	void killPlatformTimer() {
		if (m_plat_timer != -1) {
			killTimer(m_plat_timer);
			m_plat_timer = -1;
		} else
			qWarning("Platform timer not active.");
	}
	int reschedulePlatformTimer(int delay) {
		if (m_plat_timer != -1)
			killTimer(m_plat_timer);
		m_plat_timer = startTimer(delay);
	}
	Window *mainWindow() const { return m_mainWindow; }
protected:
	void timerEvent(QTimerEvent *event) {
		double next_fire_time = TBMessageHandler::GetNextMessageFireTime();
		double now = tb::TBSystem::GetTimeMS();
		if (now < next_fire_time) {
			// We timed out *before* we were supposed to (the OS is not playing nice).
			// Calling ProcessMessages now won't achieve a thing so force a reschedule
			// of the platform timer again with the same time.
			ReschedulePlatformTimer(next_fire_time, true);
			return;
		}
		TBMessageHandler::ProcessMessages();
		// If we still have things to do (because we didn't process all messages,
		// or because there are new messages), we need to rescedule, so call RescheduleTimer.
		TBSystem::RescheduleTimer(TBMessageHandler::GetNextMessageFireTime());
	}
	void char_callback(unsigned int character)
	{
		// glfw on osx seems to send us characters from the private
		// use block when using f.ex arrow keys on osx.
		if (character >= 0xE000 && character <= 0xF8FF)
			return;
	
		InvokeKey(m_mainWindow, character, TB_KEY_UNDEFINED, GetModifierKeys(), true);
		InvokeKey(m_mainWindow, character, TB_KEY_UNDEFINED, GetModifierKeys(), false);
	}
	void key_callback(int key, int scancode, int action, int mod)
	{
		MODIFIER_KEYS modifier = GetModifierKeys(mod);
		bool down = (action == QEvent::KeyPress);
		switch (key)
		{
		case Qt::Key_F1:			InvokeKey(m_mainWindow, 0, TB_KEY_F1, modifier, down); break;
		case Qt::Key_F2:			InvokeKey(m_mainWindow, 0, TB_KEY_F2, modifier, down); break;
		case Qt::Key_F3:			InvokeKey(m_mainWindow, 0, TB_KEY_F3, modifier, down); break;
		case Qt::Key_F4:			InvokeKey(m_mainWindow, 0, TB_KEY_F4, modifier, down); break;
		case Qt::Key_F5:			InvokeKey(m_mainWindow, 0, TB_KEY_F5, modifier, down); break;
		case Qt::Key_F6:			InvokeKey(m_mainWindow, 0, TB_KEY_F6, modifier, down); break;
		case Qt::Key_F7:			InvokeKey(m_mainWindow, 0, TB_KEY_F7, modifier, down); break;
		case Qt::Key_F8:			InvokeKey(m_mainWindow, 0, TB_KEY_F8, modifier, down); break;
		case Qt::Key_F9:			InvokeKey(m_mainWindow, 0, TB_KEY_F9, modifier, down); break;
		case Qt::Key_F10:			InvokeKey(m_mainWindow, 0, TB_KEY_F10, modifier, down); break;
		case Qt::Key_F11:			InvokeKey(m_mainWindow, 0, TB_KEY_F11, modifier, down); break;
		case Qt::Key_F12:			InvokeKey(m_mainWindow, 0, TB_KEY_F12, modifier, down); break;
		case Qt::Key_Left:			InvokeKey(m_mainWindow, 0, TB_KEY_LEFT, modifier, down); break;
		case Qt::Key_Up:			InvokeKey(m_mainWindow, 0, TB_KEY_UP, modifier, down); break;
		case Qt::Key_Right:			InvokeKey(m_mainWindow, 0, TB_KEY_RIGHT, modifier, down); break;
		case Qt::Key_Down:			InvokeKey(m_mainWindow, 0, TB_KEY_DOWN, modifier, down); break;
		case Qt::Key_PageUp:		InvokeKey(m_mainWindow, 0, TB_KEY_PAGE_UP, modifier, down); break;
		case Qt::Key_PageDown:		InvokeKey(m_mainWindow, 0, TB_KEY_PAGE_DOWN, modifier, down); break;
		case Qt::Key_Home:			InvokeKey(m_mainWindow, 0, TB_KEY_HOME, modifier, down); break;
		case Qt::Key_End:			InvokeKey(m_mainWindow, 0, TB_KEY_END, modifier, down); break;
		case Qt::Key_Insert:		InvokeKey(m_mainWindow, 0, TB_KEY_INSERT, modifier, down); break;
		case Qt::Key_Tab:			InvokeKey(m_mainWindow, 0, TB_KEY_TAB, modifier, down); break;
		case Qt::Key_Delete:		InvokeKey(m_mainWindow, 0, TB_KEY_DELETE, modifier, down); break;
		case Qt::Key_Backspace:	InvokeKey(m_mainWindow, 0, TB_KEY_BACKSPACE, modifier, down); break;
		case Qt::Key_Enter:		
		case Qt::Key_Return:		InvokeKey(m_mainWindow, 0, TB_KEY_ENTER, modifier, down); break;
		case Qt::Key_Escape:		InvokeKey(m_mainWindow, 0, TB_KEY_ESC, modifier, down); break;
		case Qt::Key_Menu:
			if (TBWidget::focused_widget && !down)
			{
				TBWidgetEvent ev(EVENT_TYPE_CONTEXT_MENU);
				ev.modifierkeys = modifier;
				TBWidget::focused_widget->InvokeEvent(ev);
			}
			break;
		case Qt::Key_Shift:
			key_shift = down;
			break;
		case Qt::Key_Control:
			key_ctrl = down;
			break;
		case Qt::Key_Alt:
			key_alt = down;
			break;
		case Qt::Key_Meta:
			key_super = down;
			break;
		default:
			break;
		}
	}
	void mouse_button_callback(int button, int action, int mod)
	{
		MODIFIER_KEYS modifier = GetModifierKeys(mod);
		int x = mouse_x;
		int y = mouse_y;
		if (button == Qt::LeftButton)
		{
			if (action == QEvent::MouseButtonPress)
			{
				// This is a quick fix with n-click support :)
				static double last_time = 0;
				static int last_x = 0;
				static int last_y = 0;
				static int counter = 1;
	
				double time = TBSystem::GetTimeMS();
				if (time < last_time + 600 && last_x == x && last_y == y)
					counter++;
				else
					counter = 1;
				last_x = x;
				last_y = y;
				last_time = time;
	
				GetRoot()->InvokePointerDown(x, y, counter, modifier, ShouldEmulateTouchEvent());
			}
			else
				GetRoot()->InvokePointerUp(x, y, modifier, ShouldEmulateTouchEvent());
		}
		else if (button == Qt::RightButton && action == QEvent::MouseButtonRelease)
		{
			GetRoot()->InvokePointerMove(x, y, modifier, ShouldEmulateTouchEvent());
			if (TBWidget::hovered_widget)
			{
				TBWidget::hovered_widget->ConvertFromRoot(x, y);
				TBWidgetEvent ev(EVENT_TYPE_CONTEXT_MENU, x, y, false, modifier);
				TBWidget::hovered_widget->InvokeEvent(ev);
			}
		}
	}
	void cursor_position_callback(double x, double y)
	{
		mouse_x = (int)x;
		mouse_y = (int)y;
		if (GetRoot() && !(ShouldEmulateTouchEvent() && !TBWidget::captured_widget))
			GetRoot()->InvokePointerMove(mouse_x, mouse_y, GetModifierKeys(), ShouldEmulateTouchEvent());
	}
	void scroll_callback(double x, double y)
	{
		if (GetRoot())
			GetRoot()->InvokeWheel(mouse_x, mouse_y, (int)x, -(int)y, GetModifierKeys());
	}
	void window_size_callback(int w, int h)
	{
		if (m_app)
			m_app->OnResized(w, h);
	}
	void drop_callback(int count, const char **files_utf8)
	{
		TBWidget *target = TBWidget::hovered_widget;
		if (!target)
			target = TBWidget::focused_widget;
		if (!target)
			target = GetRoot();
		if (target)
		{
			TBWidgetEventFileDrop ev;
			for (int i = 0; i < count; i++)
				ev.files.Add(new TBStr(files_utf8[i]));
			target->InvokeEvent(ev);
		}
	}
public:
	App *m_app;
	TBRendererGL *m_renderer;
	Window *m_mainWindow;
	int m_plat_timer;
	bool m_has_pending_update;
	bool m_quit_requested;
};

/** Reschedule the platform timer, or cancel it if fire_time is TB_NOT_SOON.
	If fire_time is 0, it should be fired ASAP.
	If force is true, it will ask the platform to schedule it again, even if
	the fire_time is the same as last time. */
static void ReschedulePlatformTimer(double fire_time, bool force)
{
	static double set_fire_time = -1;
	if (fire_time == TB_NOT_SOON)
	{
		set_fire_time = -1;
		AppBackendQt::Shared()->killPlatformTimer();
	}
	else if (fire_time != set_fire_time || force || fire_time == 0)
	{
		set_fire_time = fire_time;
		double delay = fire_time - tb::TBSystem::GetTimeMS();
		unsigned int idelay = (unsigned int) MAX(delay, 0.0);
		AppBackendQt::Shared()->reschedulePlatformTimer(idelay);
	}
}

static MODIFIER_KEYS GetModifierKeys()
{
	MODIFIER_KEYS code = TB_MODIFIER_NONE;
	if (key_alt)	code |= TB_ALT;
	if (key_ctrl)	code |= TB_CTRL;
	if (key_shift)	code |= TB_SHIFT;
	if (key_super)	code |= TB_SUPER;
	return code;
}

static MODIFIER_KEYS GetModifierKeys(int mod)
{
	MODIFIER_KEYS code = TB_MODIFIER_NONE;
	
	Qt::KeyboardModifiers modifiers = QApplication::keyboardModifiers();
	
	if (modifiers & Qt::AltModifier)		code |= TB_ALT;
	if (modifiers & Qt::ControlModifier)	code |= TB_CTRL;
	if (modifiers & Qt::ShiftModifier)		code |= TB_SHIFT;
	if (modifiers & Qt::MetaModifier)		code |= TB_SUPER;

	return code;
}

static bool ShouldEmulateTouchEvent()
{
	// Used to emulate that mouse events are touch events when alt, ctrl and shift are pressed.
	// This makes testing a lot easier when there is no touch screen around :)
	return (GetModifierKeys() & (TB_ALT | TB_CTRL | TB_SHIFT)) ? true : false;
}

// @return Return the upper case of a ascii charcter. Only for shortcut handling.
static int toupr_ascii(int ascii)
{
	if (ascii >= 'a' && ascii <= 'z')
		return ascii + 'A' - 'a';
	return ascii;
}

static bool InvokeShortcut(int key, SPECIAL_KEY special_key, MODIFIER_KEYS modifierkeys, bool down)
{
#ifdef TB_TARGET_MACOSX
	bool shortcut_key = (modifierkeys & TB_SUPER) ? true : false;
#else
	bool shortcut_key = (modifierkeys & TB_CTRL) ? true : false;
#endif
	if (!TBWidget::focused_widget || !down || !shortcut_key)
		return false;
	bool reverse_key = (modifierkeys & TB_SHIFT) ? true : false;
	int upper_key = toupr_ascii(key);
	TBID id;
	if (upper_key == 'X')
		id = TBIDC("cut");
	else if (upper_key == 'C' || special_key == TB_KEY_INSERT)
		id = TBIDC("copy");
	else if (upper_key == 'V' || (special_key == TB_KEY_INSERT && reverse_key))
		id = TBIDC("paste");
	else if (upper_key == 'A')
		id = TBIDC("selectall");
	else if (upper_key == 'Z' || upper_key == 'Y')
	{
		bool undo = upper_key == 'Z';
		if (reverse_key)
			undo = !undo;
		id = undo ? TBIDC("undo") : TBIDC("redo");
	}
	else if (upper_key == 'N')
		id = TBIDC("new");
	else if (upper_key == 'O')
		id = TBIDC("open");
	else if (upper_key == 'S')
		id = TBIDC("save");
	else if (upper_key == 'W')
		id = TBIDC("close");
	else if (special_key == TB_KEY_PAGE_UP)
		id = TBIDC("prev_doc");
	else if (special_key == TB_KEY_PAGE_DOWN)
		id = TBIDC("next_doc");
	else
		return false;

	TBWidgetEvent ev(EVENT_TYPE_SHORTCUT);
	ev.modifierkeys = modifierkeys;
	ev.ref_id = id;
	return TBWidget::focused_widget->InvokeEvent(ev);
}

static bool InvokeKey(Window *window, unsigned int key, SPECIAL_KEY special_key, MODIFIER_KEYS modifierkeys, bool down)
{
	if (InvokeShortcut(key, special_key, modifierkeys, down))
		return true;
	GetBackend(window)->GetRoot()->InvokeKey(key, special_key, modifierkeys, down);
	return true;
}

static void window_refresh_callback(Window *window)
{
	AppBackendQt *backend = GetBackend(window);

	backend->m_app->Process();

	backend->m_has_pending_update = false;
	// Bail out if we get here with invalid dimensions.
	// This may happen when minimizing windows (GLFW 3.0.4, Windows 8.1).
	if (backend->GetWidth() == 0 || backend->GetHeight() == 0)
		return;

	window->update();
}

bool _renderCall(Window *w, AppBackendQt *bk) { bk->m_app->RenderFrame(); }
bool _initCall(Window *w, AppBackendQt *bk) {
	App *app = bk->m_app; app->OnBackendAttached(bk, w->width(), w->height()); return app->Init();
}

bool AppBackendQt::Init(App *app)
{
	const int width = app->GetWidth() > 0 ? app->GetWidth() : 1920;
	const int height = app->GetHeight() > 0 ? app->GetHeight() : 1080;
	m_mainWindow = new Window(this);
	if (!m_mainWindow)
	{
		return false;
	}
	m_mainWindow->resize(width, height);
	m_mainWindow->setTitle(app->GetTitle());
	m_mainWindow->setCallback(_initCall, _renderCall);


#if 0 // dont/change working directory
#if defined(TB_TARGET_MACOSX) || defined(Q_OS_MAC)
	// Change working directory to the executable path. We expect it to be
	// where the demo resources are.
	char exec_path[2048];
	uint32_t exec_path_size = sizeof(exec_path);
	if (_NSGetExecutablePath(exec_path, &exec_path_size) == 0)
	{
		TBTempBuffer path;
		path.AppendPath(exec_path);
		chdir(path.GetData());
	}
#endif
#endif

	m_renderer = new TBRendererGL();
	tb_core_init(m_renderer);

	// Create the App object for our demo
	m_app = app;

	return true;
}

void AppBackendQt::Close()
{
	m_app->OnBackendDetached();
	m_app = nullptr;

	tb_core_shutdown();

	delete m_renderer;
	m_renderer = nullptr;
}

void AppBackendQt::OnAppEvent(const EVENT &ev)
{
	switch (ev)
	{
		case EVENT_PAINT_REQUEST:
			if (!m_has_pending_update)
			{
				m_has_pending_update = true;
			}
			break;
		case EVENT_QUIT_REQUEST:
			m_quit_requested = true;
			break;
		case EVENT_TITLE_CHANGED:
			m_mainWindow->setTitle(m_app->GetTitle());
			break;
		default:
			assert(!"Unhandled app event!");
	}
}

bool port_main() {

	App *app = app_create();
	AppBackendQt *backend = AppBackendQt::Shared();

	if (!backend || !backend->Init(app))
		return false;
	
	backend->mainWindow()->show();

	// Main loop
	do
	{
		qApp->processEvents();
		if (backend->m_has_pending_update)
			window_refresh_callback(backend->mainWindow());
	} while (!backend->m_quit_requested && !backend->mainWindow()->done());

	backend->Close();
	app->ShutDown();
	delete app;

	return true;
}

int main(int argc, char** argv)
{
	QApplication app(argc, argv);
	return port_main() ? 0 : 1;
}

#include "port_qt.moc"