#if defined(__linux__)
#include <core/linux/window.hpp>
#if defined(SOUNDUX_WEBVIEW_QT)
#include <QApplication>
#include <QCloseEvent>
#include <QEvent>
#include <QIcon>
#include <QKeyEvent>
#include <QMainWindow>
#include <QMetaObject>
#include <QResizeEvent>
#include <QTimer>
#include <QUrl>
#include <QWebChannel>
#include <QWebEnginePage>
#include <QWebEngineProfile>
#include <QWebEngineScript>
#include <QWebEngineScriptCollection>
#include <QWebEngineSettings>
#include <QWebEngineView>
#include <filesystem>
#include <fstream>
#include <sstream>

namespace
{
    int qtArgc = 1;
    char qtAppName[] = "soundux";
    char *qtArgv[] = {qtAppName, nullptr};

    constexpr auto bridgeScriptName = "soundux-qt-bridge";
    constexpr auto bridgeScript = R"js(
(function() {
    var pendingMessages = [];

    window.external = {
        invoke: function(message) {
            pendingMessages.push(String(message));
        }
    };

    function installExternal(channel) {
        var nativeExternal = channel.objects.external;
        window.external = {
            invoke: function(message) {
                nativeExternal.invoke(String(message));
            }
        };

        for (var i = 0; i < pendingMessages.length; i++) {
            nativeExternal.invoke(pendingMessages[i]);
        }
        pendingMessages = [];
    }

    function connectChannel() {
        if (!window.qt || !window.qt.webChannelTransport || !window.QWebChannel) {
            setTimeout(connectChannel, 0);
            return;
        }

        new QWebChannel(qt.webChannelTransport, installExternal);
    }

    connectChannel();
})();
)js";

    std::string readFile(const std::filesystem::path &path)
    {
        std::ifstream stream(path);
        std::ostringstream buffer;
        buffer << stream.rdbuf();
        return buffer.str();
    }

    std::optional<std::filesystem::path> firstExistingPath(std::initializer_list<std::filesystem::path> paths)
    {
        for (const auto &path : paths)
        {
            if (std::filesystem::exists(path))
            {
                return path;
            }
        }

        return std::nullopt;
    }
} // namespace

namespace Webview
{
    class NativeWindow : public QMainWindow
    {
        Window *owner;

      public:
        explicit NativeWindow(Window *owner) : owner(owner) {}

      protected:
        bool eventFilter(QObject *object, QEvent *event) override
        {
            if (owner && (event->type() == QEvent::KeyPress || event->type() == QEvent::KeyRelease))
            {
                auto *keyEvent = static_cast<QKeyEvent *>(event);
                if (!keyEvent->isAutoRepeat() && keyEvent->key() != Qt::Key_unknown)
                {
                    owner->handleKeyEvent(keyEvent->key(), event->type() == QEvent::KeyPress);
                }
            }

            return QMainWindow::eventFilter(object, event);
        }

        void closeEvent(QCloseEvent *event) override
        {
            if (owner && owner->handleClose())
            {
                event->ignore();
                return;
            }

            event->accept();
        }

        void resizeEvent(QResizeEvent *event) override
        {
            QMainWindow::resizeEvent(event);
            if (owner)
            {
                owner->handleResize(static_cast<std::size_t>(event->size().width()),
                                    static_cast<std::size_t>(event->size().height()));
            }
        }
    };

    ExternalBridge::ExternalBridge(Window *parent, QObject *qtParent) : QObject(qtParent), parent(parent) {}

    void ExternalBridge::invoke(const QString &message)
    {
        if (parent)
        {
            parent->handleExternalInvoke(message);
        }
    }

    Window::Window(std::size_t width, std::size_t height) : BaseWindow("", width, height)
    {
        if (!QApplication::instance())
        {
            application = std::make_unique<QApplication>(qtArgc, qtArgv);
            QApplication::setApplicationName(QStringLiteral("Soundux"));
            QApplication::setDesktopFileName(QStringLiteral("soundux"));
        }

        window = new NativeWindow(this);
        webview = new QWebEngineView(window);
        channel = new QWebChannel(webview->page());
        bridge = new ExternalBridge(this, channel);

        QApplication::instance()->installEventFilter(window);
        window->setCentralWidget(webview);
        window->resize(static_cast<int>(width), static_cast<int>(height));

        const auto executableDirectory = std::filesystem::canonical("/proc/self/exe").parent_path();
        if (auto iconPath =
                firstExistingPath({"/app/share/icons/hicolor/256x256/apps/io.github.Soundux.png",
                                   "/usr/share/pixmaps/soundux.png", executableDirectory / "assets" / "soundux.png",
                                   executableDirectory.parent_path() / "assets" / "soundux.png"}))
        {
            window->setWindowIcon(QIcon(QString::fromStdString(iconPath->string())));
        }

        channel->registerObject(QStringLiteral("external"), bridge);
        webview->page()->setWebChannel(channel);
        webview->page()->settings()->setAttribute(QWebEngineSettings::LocalContentCanAccessFileUrls, true);
        webview->page()->settings()->setAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls, true);

        if (auto webChannelScript = firstExistingPath(
                {"/usr/share/qt6/webchannel/qwebchannel.js", "/usr/share/qt5/qtwebchannel/qwebchannel.js"}))
        {
            installScript("qt-qwebchannel", readFile(*webChannelScript));
        }
        installScript(bridgeScriptName, bridgeScript);
        injectCode(setupRpc);

        QObject::connect(webview, &QWebEngineView::loadFinished,
                         [this]([[maybe_unused]] bool ok) { onNavigate(webview->url().toString().toStdString()); });
    }

    Window::Window([[maybe_unused]] const std::string &identifier, std::size_t width, std::size_t height)
        : Window(width, height)
    {
    }

    Window::~Window()
    {
        destroyNativeWindow();
    }

    void Window::destroyNativeWindow()
    {
        if (window && QApplication::instance())
        {
            QApplication::instance()->removeEventFilter(window);
        }

        if (webview)
        {
            webview->stop();
            if (webview->page())
            {
                webview->page()->setWebChannel(nullptr);
            }
            delete webview;
            webview = nullptr;
        }

        if (window)
        {
            delete window;
            window = nullptr;
        }

        channel = nullptr;
        bridge = nullptr;
    }

    void Window::runOnUiThread(std::function<void()> func)
    {
        QTimer::singleShot(0, QApplication::instance(), [func = std::move(func)] { func(); });
    }

    void Window::installScript(const std::string &name, const std::string &code)
    {
        QWebEngineScript script;
        script.setName(QString::fromStdString(name));
        script.setSourceCode(QString::fromStdString(code));
        script.setInjectionPoint(QWebEngineScript::DocumentCreation);
        script.setWorldId(QWebEngineScript::MainWorld);
        script.setRunsOnSubFrames(true);
        webview->page()->scripts().insert(script);
    }

    void Window::show()
    {
        BaseWindow::show();
        window->show();
        window->activateWindow();
    }

    void Window::hide()
    {
        BaseWindow::hide();
        window->hide();
    }

    void Window::run()
    {
        QApplication::exec();
    }

    void Window::exit()
    {
        runOnUiThread([this] {
            destroyNativeWindow();
            QApplication::quit();
        });
    }

    void Window::setUrl(std::string newUrl)
    {
        BaseWindow::setUrl(newUrl);
        webview->setUrl(QUrl(QString::fromStdString(url)));
    }

    std::string Window::getUrl()
    {
        return webview->url().toString().toStdString();
    }

    void Window::setTitle(std::string newTitle)
    {
        BaseWindow::setTitle(newTitle);
        window->setWindowTitle(QString::fromStdString(title));
    }

    void Window::setSize(std::size_t newWidth, std::size_t newHeight)
    {
        BaseWindow::setSize(newWidth, newHeight);
        window->resize(static_cast<int>(width), static_cast<int>(height));
    }

    void Window::enableDevTools([[maybe_unused]] bool state) {}

    void Window::runCode(const std::string &code)
    {
        runOnUiThread([this, code] { webview->page()->runJavaScript(QString::fromStdString(code)); });
    }

    void Window::injectCode(const std::string &code)
    {
        installScript("soundux-script-" + std::to_string(webview->page()->scripts().toList().size()), code);
    }

    void Window::handleExternalInvoke(const QString &message)
    {
        handleRawCallRequest(message.toStdString());
    }

    bool Window::handleClose()
    {
        return onClose();
    }

    void Window::handleKeyEvent(int key, bool pressed)
    {
        onKeyEvent(key, pressed);
    }

    void Window::handleResize(std::size_t width, std::size_t height)
    {
        onResize(width, height);
    }
} // namespace Webview
#else
#include <stdexcept>

Webview::Window::Window(std::size_t width, std::size_t height) : BaseWindow("", width, height)
{
    if (gtk_init_check(nullptr, nullptr) == 0)
    {
        throw std::runtime_error("Gtk init check failed");
    }

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_resizable(reinterpret_cast<GtkWindow *>(window), true);
    gtk_window_set_default_size(reinterpret_cast<GtkWindow *>(window), static_cast<int>(width),
                                static_cast<int>(height));
    gtk_window_set_position(reinterpret_cast<GtkWindow *>(window), GTK_WIN_POS_CENTER);

    GtkWidget *scrollView = gtk_scrolled_window_new(nullptr, nullptr);
    gtk_container_add(reinterpret_cast<GtkContainer *>(window), scrollView);

    WebKitUserContentManager *contentManager = webkit_user_content_manager_new();
    webkit_user_content_manager_register_script_message_handler(contentManager, "external");

    webview = webkit_web_view_new_with_user_content_manager(contentManager);
    gtk_container_add(reinterpret_cast<GtkContainer *>(scrollView), webview);

    g_signal_connect(window, "destroy", reinterpret_cast<GCallback>(destroy), this);
    g_signal_connect(window, "delete_event", reinterpret_cast<GCallback>(closed), this);
    g_signal_connect(window, "configure-event", reinterpret_cast<GCallback>(resize), this);

#if defined(WEBVIEW_EMBEDDED)
    webkit_web_context_register_uri_scheme(webkit_web_context_get_default(), "embedded", onUriRequested, this, nullptr);
#endif

    g_signal_connect(contentManager, "script-message-received::external", reinterpret_cast<GCallback>(messageReceived),
                     this);
    g_signal_connect(reinterpret_cast<GObject *>(webview), "load-changed", reinterpret_cast<GCallback>(loadChanged),
                     this);
    g_signal_connect(reinterpret_cast<GObject *>(webview), "context-menu", reinterpret_cast<GCallback>(contextMenu),
                     this);

    injectCode("window.external={invoke:arg=>window.webkit."
               "messageHandlers.external.postMessage(arg)};");
    injectCode(setupRpc);

    gtk_widget_grab_focus(webview);
    gtk_widget_show_all(scrollView);
}

Webview::Window::Window([[maybe_unused]] const std::string &identifier, std::size_t width, std::size_t height)
    : Window(width, height)
{
}

void Webview::Window::show()
{
    BaseWindow::show();
    gtk_widget_grab_focus(window);
    gtk_widget_show(window);
}

void Webview::Window::hide()
{
    BaseWindow::hide();
    gtk_widget_hide(window);
}

void Webview::Window::run()
{
    while (window)
    {
        gtk_main_iteration_do(true);
    }
}

void Webview::Window::exit()
{
    gtk_widget_destroy(window);
}

void Webview::Window::setUrl(std::string newUrl)
{
    BaseWindow::setUrl(newUrl);
    webkit_web_view_load_uri(reinterpret_cast<WebKitWebView *>(webview), url.c_str());
}

void Webview::Window::setTitle(std::string newTitle)
{
    BaseWindow::setTitle(newTitle);
    gtk_window_set_title(reinterpret_cast<GtkWindow *>(window), title.c_str());
}

void Webview::Window::setSize(std::size_t newWidth, std::size_t newHeight)
{
    BaseWindow::setSize(newWidth, newHeight);
    gtk_window_resize(reinterpret_cast<GtkWindow *>(window), static_cast<int>(width), static_cast<int>(height));
}

void Webview::Window::enableDevTools(bool state)
{
    WebKitSettings *settings = webkit_web_view_get_settings(reinterpret_cast<WebKitWebView *>(webview)); // NOLINT
    webkit_settings_set_enable_write_console_messages_to_stdout(settings, state);
    webkit_settings_set_enable_developer_extras(settings, state);
}

void Webview::Window::runOnIdle(std::function<void()> func)
{
    auto *funcPtr = new std::function<void()>(std::move(func));

    g_idle_add(
        [](gpointer data) -> gboolean {
            auto *func = reinterpret_cast<std::function<void()> *>(data);
            (*func)();

            delete func;
            return FALSE;
        },
        funcPtr);
}

void Webview::Window::runCode(const std::string &code)
{
    runOnIdle([this, code] {
        webkit_web_view_run_javascript(reinterpret_cast<WebKitWebView *>(webview), formatCode(code).c_str(), nullptr,
                                       nullptr, nullptr);
    });
}

void Webview::Window::injectCode(const std::string &code)
{
    auto *manager = webkit_web_view_get_user_content_manager(reinterpret_cast<WebKitWebView *>(webview));
    webkit_user_content_manager_add_script(
        manager, webkit_user_script_new(formatCode(code).c_str(), WEBKIT_USER_CONTENT_INJECT_ALL_FRAMES,
                                        WEBKIT_USER_SCRIPT_INJECT_AT_DOCUMENT_START, nullptr, nullptr));
}

gboolean Webview::Window::contextMenu([[maybe_unused]] WebKitWebView *webkitwebview, [[maybe_unused]] GtkWidget *widget,
                                      [[maybe_unused]] WebKitHitTestResultContext *result,
                                      [[maybe_unused]] gboolean with_keyboard, gpointer arg)
{
    auto *webview = reinterpret_cast<Window *>(arg);
    if (webview->isContextMenuAllowed)
    {
        return 0;
    }
    return 1;
}

void Webview::Window::destroy([[maybe_unused]] GtkWidget *widget, gpointer arg)
{
    auto *webview = reinterpret_cast<Window *>(arg);
    webview->window = nullptr;
}

void Webview::Window::loadChanged(WebKitWebView *webkitwebview, [[maybe_unused]] WebKitLoadEvent event,
                                  [[maybe_unused]] gpointer arg)
{
    if (event == WEBKIT_LOAD_FINISHED)
    {
        auto *webview = reinterpret_cast<Window *>(arg);
        webview->onNavigate(webkit_web_view_get_uri(webkitwebview));
    }
}

gboolean Webview::Window::resize([[maybe_unused]] WebKitWebView *webkitwebview, GdkEvent *event, gpointer arg)
{
    auto *gtkEvent = reinterpret_cast<GdkEventConfigure *>(event);
    auto *webview = reinterpret_cast<Window *>(arg);

    webview->onResize(gtkEvent->width, gtkEvent->height);
    return 0;
}

gboolean Webview::Window::closed([[maybe_unused]] GtkWidget *widget, [[maybe_unused]] GdkEvent *event, gpointer data)
{
    auto *webview = reinterpret_cast<Window *>(data);

    if (webview->onClose())
    {
        return TRUE;
    }

    return FALSE;
}

void Webview::Window::messageReceived([[maybe_unused]] WebKitUserContentManager *contentManager,
                                      WebKitJavascriptResult *result, gpointer arg)
{
    auto *webview = reinterpret_cast<Window *>(arg);

    auto *value = webkit_javascript_result_get_js_value(result);
    webview->handleRawCallRequest(jsc_value_to_string(value));
}

std::string Webview::Window::getUrl()
{
    return webkit_web_view_get_uri(reinterpret_cast<WebKitWebView *>(webview));
}

#if defined(WEBVIEW_EMBEDDED)
void Webview::Window::onUriRequested(WebKitURISchemeRequest *request, [[maybe_unused]] gpointer userData)
{
    auto *webview = reinterpret_cast<Window *>(userData);
    GInputStream *stream = nullptr;

    std::string uri = webkit_uri_scheme_request_get_uri(request);
    if (uri.size() > 11)
    {
        uri = uri.substr(11);
        auto fileName = uri.substr(uri.find_last_of('/') + 1);
        auto resourceData = webview->getResource(fileName);

        if (resourceData.data)
        {
            stream =
                g_memory_input_stream_new_from_data(resourceData.data, static_cast<long>(resourceData.size), g_free);
            webkit_uri_scheme_request_finish(request, stream, static_cast<long>(resourceData.size), nullptr);
        }
    }
}
#endif
#endif
#endif
