#if (defined(__linux__) && __has_include(<X11/Xlib.h>)) || defined(Q_MOC_RUN)
#include "../hotkeys.hpp"
#include <QCoreApplication>
#include <QDBusArgument>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusMetaType>
#include <QDBusObjectPath>
#include <QDBusReply>
#include <QEventLoop>
#include <QKeySequence>
#include <QList>
#include <QObject>
#include <QString>
#include <QTimer>
#include <Qt>
#include <QVariantMap>
#include <X11/X.h>
#include <X11/XKBlib.h>
#include <X11/Xlib.h>
#include <X11/extensions/XI2.h>
#include <X11/extensions/XInput2.h>
#include <X11/extensions/XTest.h>
#ifdef None
#undef None
#endif
#ifdef signals
#undef signals
#endif
#include <chrono>
#include <core/global/globals.hpp>
#include <cstdlib>
#include <fancy.hpp>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <thread>

using namespace std::chrono_literals;

namespace
{
    using PortalShortcut = QPair<QString, QVariantMap>;
    using PortalShortcutList = QList<PortalShortcut>;

    QDBusArgument &operator<<(QDBusArgument &argument, const PortalShortcut &shortcut)
    {
        argument.beginStructure();
        argument << shortcut.first << shortcut.second;
        argument.endStructure();
        return argument;
    }

    const QDBusArgument &operator>>(const QDBusArgument &argument, PortalShortcut &shortcut)
    {
        argument.beginStructure();
        argument >> shortcut.first >> shortcut.second;
        argument.endStructure();
        return argument;
    }
} // namespace

Q_DECLARE_METATYPE(PortalShortcut)
Q_DECLARE_METATYPE(PortalShortcutList)

namespace Soundux
{
namespace Objects
{
    Display *display;

    static bool isWaylandSession()
    {
        const auto *waylandDisplay = std::getenv("WAYLAND_DISPLAY"); // NOLINT
        const auto *sessionType = std::getenv("XDG_SESSION_TYPE");   // NOLINT

        return (waylandDisplay && std::string(waylandDisplay).length() > 0) ||
               (sessionType && std::string(sessionType) == "wayland");
    }

    constexpr auto portalService = "org.freedesktop.portal.Desktop";
    constexpr auto portalPath = "/org/freedesktop/portal/desktop";
    constexpr auto globalShortcutsInterface = "org.freedesktop.portal.GlobalShortcuts";
    constexpr auto requestInterface = "org.freedesktop.portal.Request";
    constexpr auto sessionInterface = "org.freedesktop.portal.Session";

    class PortalResponseWaiter : public QObject
    {
        Q_OBJECT

        bool completed = false;
        std::uint32_t responseCode = 1;
        QVariantMap responseResults;
        QEventLoop loop;

      public:
        std::optional<QVariantMap> wait(const QDBusObjectPath &requestPath, int timeoutMs = 120000)
        {
            const auto connected = QDBusConnection::sessionBus().connect(
                portalService, requestPath.path(), requestInterface, QStringLiteral("Response"), this,
                SLOT(onResponse(uint, QVariantMap)));
            if (!connected)
            {
                return std::nullopt;
            }

            QTimer::singleShot(timeoutMs, &loop, &QEventLoop::quit);
            loop.exec();

            QDBusConnection::sessionBus().disconnect(portalService, requestPath.path(), requestInterface,
                                                     QStringLiteral("Response"), this,
                                                     SLOT(onResponse(uint, QVariantMap)));
            if (!completed || responseCode != 0)
            {
                return std::nullopt;
            }

            return responseResults;
        }

      public slots:
        void onResponse(uint response, const QVariantMap &results)
        {
            completed = true;
            responseCode = response;
            responseResults = results;
            loop.quit();
        }
    };

    static std::string getPortalKeyName(int key)
    {
        if (key >= Qt::Key_A && key <= Qt::Key_Z)
        {
            return std::string(1, static_cast<char>('a' + (key - Qt::Key_A)));
        }
        if (key >= Qt::Key_0 && key <= Qt::Key_9)
        {
            return std::string(1, static_cast<char>('0' + (key - Qt::Key_0)));
        }
        if (key >= Qt::Key_F1 && key <= Qt::Key_F35)
        {
            return "F" + std::to_string((key - Qt::Key_F1) + 1);
        }

        switch (key)
        {
            case Qt::Key_Escape:
                return "Escape";
            case Qt::Key_Return:
            case Qt::Key_Enter:
                return "Return";
            case Qt::Key_Backspace:
                return "BackSpace";
            case Qt::Key_Tab:
                return "Tab";
            case Qt::Key_Space:
                return "space";
            case Qt::Key_Delete:
                return "Delete";
            case Qt::Key_Insert:
                return "Insert";
            case Qt::Key_Home:
                return "Home";
            case Qt::Key_End:
                return "End";
            case Qt::Key_PageUp:
                return "Page_Up";
            case Qt::Key_PageDown:
                return "Page_Down";
            case Qt::Key_Left:
                return "Left";
            case Qt::Key_Right:
                return "Right";
            case Qt::Key_Up:
                return "Up";
            case Qt::Key_Down:
                return "Down";
            case Qt::Key_Minus:
                return "minus";
            case Qt::Key_Plus:
                return "plus";
            case Qt::Key_Equal:
                return "equal";
            case Qt::Key_Comma:
                return "comma";
            case Qt::Key_Period:
                return "period";
            case Qt::Key_Slash:
                return "slash";
            case Qt::Key_Backslash:
                return "backslash";
            case Qt::Key_Semicolon:
                return "semicolon";
            case Qt::Key_Apostrophe:
                return "apostrophe";
            case Qt::Key_BracketLeft:
                return "bracketleft";
            case Qt::Key_BracketRight:
                return "bracketright";
            case Qt::Key_QuoteLeft:
                return "grave";
            default:
                return "";
        }
    }

    static std::optional<std::string> getPortalTrigger(const std::vector<int> &keys)
    {
        bool ctrl = false;
        bool alt = false;
        bool shift = false;
        bool logo = false;
        std::string keyName;

        for (const auto &key : keys)
        {
            switch (key)
            {
                case Qt::Key_Control:
                    ctrl = true;
                    continue;
                case Qt::Key_Alt:
                    alt = true;
                    continue;
                case Qt::Key_Shift:
                    shift = true;
                    continue;
                case Qt::Key_Meta:
                    logo = true;
                    continue;
                default:
                    keyName = getPortalKeyName(key);
                    break;
            }
        }

        if (keyName.empty())
        {
            return std::nullopt;
        }

        std::string trigger;
        if (ctrl)
        {
            trigger += "CTRL+";
        }
        if (alt)
        {
            trigger += "ALT+";
        }
        if (shift)
        {
            trigger += "SHIFT+";
        }
        if (logo)
        {
            trigger += "LOGO+";
        }

        trigger += keyName;
        return trigger;
    }

    class GlobalShortcutsPortal : public QObject
    {
        Q_OBJECT

        Hotkeys *owner;
        QDBusObjectPath sessionHandle;
        std::map<QString, std::vector<int>> actionKeys;
        std::string lastSignature;
        bool activationConnected = false;

        static QString makeToken(const QString &prefix)
        {
            static std::uint32_t counter = 0;
            return prefix + QString::number(++counter);
        }

        static bool portalAvailable()
        {
            QDBusInterface portal(portalService, portalPath, globalShortcutsInterface, QDBusConnection::sessionBus());
            return portal.isValid();
        }

        void closeSession()
        {
            if (sessionHandle.path().isEmpty())
            {
                return;
            }

            QDBusInterface session(portalService, sessionHandle.path(), sessionInterface, QDBusConnection::sessionBus());
            session.call(QStringLiteral("Close"));
            sessionHandle = {};
            actionKeys.clear();
        }

        std::optional<QDBusObjectPath> createSession()
        {
            QDBusInterface portal(portalService, portalPath, globalShortcutsInterface, QDBusConnection::sessionBus());
            QVariantMap options;
            options.insert(QStringLiteral("handle_token"), makeToken(QStringLiteral("soundux_create_")));
            options.insert(QStringLiteral("session_handle_token"), makeToken(QStringLiteral("soundux_session_")));

            QDBusReply<QDBusObjectPath> reply = portal.call(QStringLiteral("CreateSession"), options);
            if (!reply.isValid())
            {
                Fancy::fancy.logTime().warning()
                    << "Failed to create Wayland global shortcuts session: "
                    << reply.error().message().toStdString() << std::endl;
                return std::nullopt;
            }

            PortalResponseWaiter waiter;
            auto result = waiter.wait(reply.value());
            if (!result)
            {
                Fancy::fancy.logTime().warning()
                    << "Wayland global shortcuts session was not approved by the portal" << std::endl;
                return std::nullopt;
            }

            auto rawSession = result->value(QStringLiteral("session_handle")).toString();
            if (rawSession.isEmpty())
            {
                Fancy::fancy.logTime().warning()
                    << "Wayland global shortcuts portal returned an empty session" << std::endl;
                return std::nullopt;
            }

            return QDBusObjectPath(rawSession);
        }

        bool bindShortcuts(const PortalShortcutList &shortcuts)
        {
            QDBusInterface portal(portalService, portalPath, globalShortcutsInterface, QDBusConnection::sessionBus());
            QVariantMap options;
            options.insert(QStringLiteral("handle_token"), makeToken(QStringLiteral("soundux_bind_")));

            QDBusReply<QDBusObjectPath> reply =
                portal.call(QStringLiteral("BindShortcuts"), QVariant::fromValue(sessionHandle),
                            QVariant::fromValue(shortcuts), QString(), options);
            if (!reply.isValid())
            {
                Fancy::fancy.logTime().warning()
                    << "Failed to bind Wayland global shortcuts: " << reply.error().message().toStdString()
                    << std::endl;
                return false;
            }

            PortalResponseWaiter waiter;
            if (!waiter.wait(reply.value()))
            {
                Fancy::fancy.logTime().warning()
                    << "Wayland global shortcuts were not approved by the portal" << std::endl;
                return false;
            }

            return true;
        }

        void ensureActivationSignal()
        {
            if (activationConnected)
            {
                return;
            }

            activationConnected = QDBusConnection::sessionBus().connect(
                portalService, portalPath, globalShortcutsInterface, QStringLiteral("Activated"), this,
                SLOT(onActivated(QDBusObjectPath, QString, qulonglong, QVariantMap)));
            QDBusConnection::sessionBus().connect(portalService, portalPath, globalShortcutsInterface,
                                                 QStringLiteral("Deactivated"), this,
                                                 SLOT(onDeactivated(QDBusObjectPath, QString, qulonglong, QVariantMap)));
        }

      public:
        explicit GlobalShortcutsPortal(Hotkeys *owner) : owner(owner)
        {
            qDBusRegisterMetaType<PortalShortcut>();
            qDBusRegisterMetaType<PortalShortcutList>();
        }

        ~GlobalShortcutsPortal() override
        {
            closeSession();
        }

        void refresh()
        {
            if (!isWaylandSession() || !QCoreApplication::instance() || !portalAvailable())
            {
                return;
            }

            PortalShortcutList shortcuts;
            std::map<QString, std::vector<int>> newActionKeys;
            std::string signature;

            auto addShortcut = [&](const QString &id, const std::string &description, const std::vector<int> &keys) {
                if (keys.empty())
                {
                    return;
                }

                QVariantMap properties;
                properties.insert(QStringLiteral("description"), QString::fromStdString(description));
                if (auto trigger = getPortalTrigger(keys))
                {
                    properties.insert(QStringLiteral("preferred_trigger"), QString::fromStdString(*trigger));
                    signature += id.toStdString() + ":" + *trigger + ";";
                }
                else
                {
                    signature += id.toStdString() + ":;";
                }

                shortcuts.emplace_back(id, properties);
                newActionKeys.emplace(id, keys);
            };

            addShortcut(QStringLiteral("stop"), "Stop all sounds", Globals::gSettings.stopHotkey);

            auto scopedSounds = Globals::gSounds.scoped();
            for (const auto &[id, soundRef] : *scopedSounds)
            {
                const auto &sound = soundRef.get();
                addShortcut(QStringLiteral("sound_") + QString::number(id), "Play " + sound.name, sound.hotkeys);
            }

            if (signature == lastSignature)
            {
                return;
            }

            closeSession();

            if (shortcuts.empty())
            {
                lastSignature = signature;
                return;
            }

            auto session = createSession();
            if (!session)
            {
                return;
            }

            sessionHandle = *session;
            actionKeys = std::move(newActionKeys);
            ensureActivationSignal();

            if (bindShortcuts(shortcuts))
            {
                lastSignature = signature;
                Fancy::fancy.logTime().success()
                    << "Registered " << shortcuts.size() << " Wayland global shortcut action(s)" << std::endl;
            }
            else
            {
                closeSession();
            }
        }

      public slots:
        void onActivated(const QDBusObjectPath &session, const QString &shortcutId, qulonglong,
                         const QVariantMap &)
        {
            if (session.path() != sessionHandle.path() || !actionKeys.count(shortcutId))
            {
                return;
            }

            for (const auto &key : actionKeys.at(shortcutId))
            {
                owner->onKeyDown(key);
            }
        }

        void onDeactivated(const QDBusObjectPath &session, const QString &shortcutId, qulonglong,
                           const QVariantMap &)
        {
            if (session.path() != sessionHandle.path() || !actionKeys.count(shortcutId))
            {
                return;
            }

            const auto &keys = actionKeys.at(shortcutId);
            for (auto it = keys.rbegin(); it != keys.rend(); ++it)
            {
                owner->onKeyUp(*it);
            }
        }
    };

    std::unique_ptr<GlobalShortcutsPortal> globalShortcutsPortal;

    static std::string getQtKeyName(int key)
    {
        switch (key)
        {
            case Qt::Key_Control:
                return "Ctrl";
            case Qt::Key_Shift:
                return "Shift";
            case Qt::Key_Alt:
                return "Alt";
            case Qt::Key_Meta:
                return "Meta";
            case Qt::Key_Escape:
                return "Esc";
            case Qt::Key_Return:
            case Qt::Key_Enter:
                return "Enter";
            case Qt::Key_Backspace:
                return "Backspace";
            case Qt::Key_Tab:
                return "Tab";
            case Qt::Key_Space:
                return "Space";
            case Qt::Key_Delete:
                return "Delete";
            case Qt::Key_Insert:
                return "Insert";
            case Qt::Key_Home:
                return "Home";
            case Qt::Key_End:
                return "End";
            case Qt::Key_PageUp:
                return "PageUp";
            case Qt::Key_PageDown:
                return "PageDown";
            case Qt::Key_Left:
                return "Left";
            case Qt::Key_Right:
                return "Right";
            case Qt::Key_Up:
                return "Up";
            case Qt::Key_Down:
                return "Down";
            default:
                break;
        }

        auto name = QKeySequence(key).toString(QKeySequence::NativeText).toStdString();
        if (!name.empty())
        {
            return name;
        }

        return "KEY_" + std::to_string(key);
    }

    void Hotkeys::listen()
    {
        if (isWaylandSession())
        {
            Fancy::fancy.logTime().message()
                << "Wayland session detected - X11 global hotkeys are disabled" << std::endl;
            return;
        }

        auto *displayenv = std::getenv("DISPLAY"); // NOLINT
        auto *x11Display = XOpenDisplay(displayenv);

        if (!x11Display)
        {
            Fancy::fancy.logTime().warning() << "DISPLAY is not set, defaulting to :0" << std::endl;
            if (!(x11Display = XOpenDisplay(":0")))
            {
                Fancy::fancy.logTime().failure() << "Could not open X11 Display" << std::endl;
                return;
            }
        }
        else
        {
            Fancy::fancy.logTime().message() << "Using DISPLAY " << displayenv << std::endl;
        }
        display = x11Display;

        int major_op = 0, event_rtn = 0, ext_rtn = 0;
        if (!XQueryExtension(display, "XInputExtension", &major_op, &event_rtn, &ext_rtn))
        {
            Fancy::fancy.logTime().failure() << "Failed to find XInputExtension" << std::endl;
            return;
        }

        ::Window root = DefaultRootWindow(display); // NOLINT

        XIEventMask mask;
        mask.deviceid = XIAllMasterDevices;
        mask.mask_len = XIMaskLen(XI_LASTEVENT);
        mask.mask = static_cast<unsigned char *>(calloc(mask.mask_len, sizeof(char)));

        XISetMask(mask.mask, XI_RawKeyPress);
        XISetMask(mask.mask, XI_RawKeyRelease);
        XISetMask(mask.mask, XI_RawButtonPress);
        XISetMask(mask.mask, XI_RawButtonRelease);
        XISelectEvents(display, root, &mask, 1);

        XSync(display, 0);
        free(mask.mask);

        while (!kill)
        {
            if (XPending(display) != 0)
            {
                XEvent event;
                XNextEvent(display, &event);
                auto *cookie = reinterpret_cast<XGenericEventCookie *>(&event.xcookie);

                if (XGetEventData(display, cookie) && cookie->type == GenericEvent && cookie->extension == major_op &&
                    (cookie->evtype == XI_RawKeyPress || cookie->evtype == XI_RawKeyRelease ||
                     cookie->evtype == XI_RawButtonPress || cookie->evtype == XI_RawButtonRelease))
                {
                    auto *data = reinterpret_cast<XIRawEvent *>(cookie->data);
                    auto key = data->detail;

                    if (key == 1)
                        continue;

                    if (cookie->evtype == XI_RawKeyPress || cookie->evtype == XI_RawButtonPress)
                    {
                        onKeyDown(key);
                    }
                    else if (cookie->evtype == XI_RawKeyRelease || cookie->evtype == XI_RawButtonRelease)
                    {
                        onKeyUp(key);
                    }
                }
            }
            else
            {
                std::this_thread::sleep_for(100ms);
            }
        }
    }

    std::string Hotkeys::getKeyName(const int &key)
    {
        if (!display)
        {
            if (isWaylandSession())
            {
                return getQtKeyName(key);
            }
            return "KEY_" + std::to_string(key);
        }

        // TODO(curve): There is no Keysym for the mouse buttons and I couldn't find any way to get the name for the
        // mouse buttons so they'll just be named KEY_1 (1 is the Keycode). Maybe someone will be able to help me but I
        // just can't figure it out

        KeySym s = XkbKeycodeToKeysym(display, key, 0, 0);

        if (s == NoSymbol)
        {
            return "KEY_" + std::to_string(key);
        }

        auto *str = XKeysymToString(s);
        if (str == nullptr)
        {
            return "KEY_" + std::to_string(key);
        }

        return str;
    }

    void Hotkeys::stop()
    {
        kill = true;
        globalShortcutsPortal.reset();
        if (listener.joinable())
        {
            listener.join();
        }
    }

    void Hotkeys::pressKeys(const std::vector<int> &keys)
    {
        if (!display)
        {
            return;
        }

        keysToPress = keys;
        for (const auto &key : keys)
        {
            XTestFakeKeyEvent(display, key, True, 0);
        }
    }

    void Hotkeys::releaseKeys(const std::vector<int> &keys)
    {
        if (!display)
        {
            return;
        }

        keysToPress.clear();
        for (const auto &key : keys)
        {
            XTestFakeKeyEvent(display, key, False, 0);
        }
    }

    void Hotkeys::refreshWaylandGlobalShortcuts()
    {
        if (!isWaylandSession() || !QCoreApplication::instance())
        {
            return;
        }

        if (!globalShortcutsPortal)
        {
            globalShortcutsPortal = std::make_unique<GlobalShortcutsPortal>(this);
        }

        globalShortcutsPortal->refresh();
    }
} // namespace Objects
} // namespace Soundux

#include "x11.moc"

#endif
