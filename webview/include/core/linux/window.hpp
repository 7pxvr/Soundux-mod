#pragma once
#if defined(__linux__)
#include <core/basewindow.hpp>
#if defined(SOUNDUX_WEBVIEW_QT)
#include <QApplication>
#include <QObject>
#include <QString>
#include <memory>

class QCloseEvent;
class QResizeEvent;
class QWebChannel;
class QWebEngineView;

namespace Webview
{
    class NativeWindow;
    class Window;

    class ExternalBridge : public QObject
    {
        Q_OBJECT

        Window *parent;

      public:
        explicit ExternalBridge(Window *parent, QObject *qtParent = nullptr);

        Q_INVOKABLE void invoke(const QString &message);
    };

    class Window : public BaseWindow
    {
        std::unique_ptr<QApplication> application;
        NativeWindow *window;
        QWebEngineView *webview;
        QWebChannel *channel;
        ExternalBridge *bridge;

        void installScript(const std::string &name, const std::string &code);
        void runOnUiThread(std::function<void()> func);

      public:
        Window(std::size_t width, std::size_t height);
        Window(const std::string &identifier, std::size_t width,
               std::size_t height); //* Identifier is not required on linux.
        ~Window();

        void hide() override;
        void show() override;

        void run() override;
        void exit() override;

        std::string getUrl() override;
        void setUrl(std::string newUrl) override;
        void setTitle(std::string newTitle) override;
        void setSize(std::size_t newWidth, std::size_t newHeight) override;

        void enableDevTools(bool state) override;
        void runCode(const std::string &code) override;
        void injectCode(const std::string &code) override;

        void handleExternalInvoke(const QString &message);
        bool handleClose();
        void handleKeyEvent(int key, bool pressed);
        void handleResize(std::size_t width, std::size_t height);

      private:
        void destroyNativeWindow();
    };
} // namespace Webview
#else
#include <gtk/gtk.h>
#include <webkit2/webkit2.h>

namespace Webview
{
    class Window : public BaseWindow
    {
        GtkWidget *window;
        GtkWidget *webview;

        static void destroy(GtkWidget *, gpointer);
        static gboolean closed(GtkWidget *, GdkEvent *, gpointer);
        static gboolean resize(WebKitWebView *, GdkEvent *, gpointer);

#if defined(WEBVIEW_EMBEDDED)
        static void onUriRequested(WebKitURISchemeRequest *, gpointer);
#endif

        static void loadChanged(WebKitWebView *, WebKitLoadEvent, gpointer);
        static void messageReceived(WebKitUserContentManager *, WebKitJavascriptResult *, gpointer);
        static gboolean contextMenu(WebKitWebView *, GtkWidget *, WebKitHitTestResultContext *, gboolean, gpointer);

      private:
        void runOnIdle(std::function<void()>);

      public:
        Window(std::size_t width, std::size_t height);
        Window(const std::string &identifier, std::size_t width,
               std::size_t height); //* Identifier is not required on linux.

        void hide() override;
        void show() override;

        void run() override;
        void exit() override;

        std::string getUrl() override;
        void setUrl(std::string newUrl) override;
        void setTitle(std::string newTitle) override;
        void setSize(std::size_t newWidth, std::size_t newHeight) override;

        void enableDevTools(bool state) override;
        void runCode(const std::string &code) override;
        void injectCode(const std::string &code) override;
    };
} // namespace Webview
#endif
#endif
