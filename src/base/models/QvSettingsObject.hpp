#pragma once
#include "base/models/CoreObjectModels.hpp"
#include "base/models/QvConfigIdentifier.hpp"
#include "libs/QJsonStruct/QJsonStruct.hpp"

#include <chrono>

const int QV2RAY_CONFIG_VERSION = 13;

namespace Qv2ray::base::config
{
    struct Qv2rayConfig_ForwardProxy
    {
        bool enableForwardProxy;
        QString type;
        QString serverAddress;
        int port;
        bool useAuth;
        QString username;
        QString password;
        Qv2rayConfig_ForwardProxy()
            : enableForwardProxy(false), type("http"), serverAddress("127.0.0.1"), port(8008), useAuth(false), username(), password(){};
        JSONSTRUCT_REGISTER(Qv2rayConfig_ForwardProxy, F(enableForwardProxy, type, serverAddress, port, useAuth, username, password))
    };
    struct Qv2rayConfig_SystemProxy
    {
        bool setSystemProxy;
        Qv2rayConfig_SystemProxy() : setSystemProxy(true){};
        JSONSTRUCT_REGISTER(Qv2rayConfig_SystemProxy, F(setSystemProxy))
    };

    struct Qv2rayConfig_SocksInbound
    {
        int port;
        bool useAuth;
        bool enableUDP;
        QString localIP;
        objects::AccountObject account;
        bool sniffing;
        Qv2rayConfig_SocksInbound() : port(1089), useAuth(false), enableUDP(true), localIP("127.0.0.1"), account(), sniffing(false){};
        JSONSTRUCT_REGISTER(Qv2rayConfig_SocksInbound, F(port, useAuth, enableUDP, localIP, account, sniffing))
    };

    struct Qv2rayConfig_HttpInbound
    {
        int port;
        bool useAuth;
        objects::AccountObject account;
        bool sniffing;
        Qv2rayConfig_HttpInbound() : port(8889), useAuth(false), account(), sniffing(false){};
        JSONSTRUCT_REGISTER(Qv2rayConfig_HttpInbound, F(port, useAuth, account, sniffing))
    };

    struct Qv2rayConfig_tProxy
    {
        QString tProxyIP;
        int port;
        bool hasTCP;
        bool hasUDP;
        bool followRedirect;
        QString mode;
        bool dnsIntercept;
        Qv2rayConfig_tProxy()
            : tProxyIP("127.0.0.1"), port(12345), hasTCP(true), hasUDP(false), followRedirect(true), mode("tproxy"), dnsIntercept(true){};
        JSONSTRUCT_REGISTER(Qv2rayConfig_tProxy, F(tProxyIP, port, hasTCP, hasUDP, followRedirect, mode, dnsIntercept))
    };

    struct Qv2rayConfig_Inbounds
    {
        QString listenip;
        bool useSocks;
        bool useHTTP;
        bool useTPROXY;
        //
        Qv2rayConfig_tProxy tProxySettings;
        Qv2rayConfig_HttpInbound httpSettings;
        Qv2rayConfig_SocksInbound socksSettings;
        Qv2rayConfig_SystemProxy systemProxySettings;
        Qv2rayConfig_Inbounds() : listenip("127.0.0.1"), useSocks(true), useHTTP(true), useTPROXY(false){};

        JSONSTRUCT_REGISTER(Qv2rayConfig_Inbounds,                     //
                            F(listenip, useSocks, useHTTP, useTPROXY), //
                            F(tProxySettings, httpSettings, socksSettings, systemProxySettings))
    };

    struct Qv2rayConfig_Outbounds
    {
        int mark;
        Qv2rayConfig_Outbounds() : mark(255){};
        JSONSTRUCT_REGISTER(Qv2rayConfig_Outbounds, F(mark))
    };

    struct Qv2rayConfig_UI
    {
        QString theme;
        QString language;
        QList<ConnectionGroupPair> recentConnections;
        bool quietMode;
        bool useDarkTheme;
        bool useDarkTrayIcon;
        int maximumLogLines;
        int maxJumpListCount;
        Qv2rayConfig_UI()
            : theme("Fusion"), language("en_US"), useDarkTheme(false), useDarkTrayIcon(true), maximumLogLines(500), maxJumpListCount(20){};
        JSONSTRUCT_REGISTER(Qv2rayConfig_UI,
                            F(theme, language, quietMode, useDarkTheme, useDarkTrayIcon, maximumLogLines, maxJumpListCount, recentConnections))
    };

    struct Qv2rayConfig_Routing
    {
        struct Qv2rayRouteConfig_Impl
        {
            QList<QString> direct;
            QList<QString> block;
            QList<QString> proxy;
            Qv2rayRouteConfig_Impl(){};
            friend bool operator==(const Qv2rayRouteConfig_Impl &left, const Qv2rayRouteConfig_Impl &right)
            {
                return left.direct == right.direct && left.block == right.block && left.proxy == right.proxy;
            }
            Qv2rayRouteConfig_Impl(const QList<QString> &_direct, const QList<QString> &_block, const QList<QString> &_proxy)
                : direct(_direct), block(_block), proxy(_proxy){};
            JSONSTRUCT_REGISTER(Qv2rayRouteConfig_Impl, F(proxy, block, direct))
        };
        QString domainStrategy;
        Qv2rayRouteConfig_Impl domains;
        Qv2rayRouteConfig_Impl ips;
        friend bool operator==(const Qv2rayConfig_Routing &left, const Qv2rayConfig_Routing &right)
        {
            return left.domainStrategy == right.domainStrategy && left.domains == right.domains && left.ips == right.ips;
        }
        Qv2rayConfig_Routing(){};
        Qv2rayConfig_Routing(const Qv2rayRouteConfig_Impl &_domains, const Qv2rayRouteConfig_Impl &_ips, const QString &ds)
            : domainStrategy(ds), domains(_domains), ips(_ips){};
        JSONSTRUCT_REGISTER(Qv2rayConfig_Routing, F(domainStrategy, domains, ips))
    };

    struct Qv2rayConfig_Plugin
    {
        QMap<QString, bool> pluginStates;
        bool v2rayIntegration;
        int portAllocationStart;
        Qv2rayConfig_Plugin() : pluginStates(), v2rayIntegration(true), portAllocationStart(15000){};
        JSONSTRUCT_REGISTER(Qv2rayConfig_Plugin, F(pluginStates, v2rayIntegration, portAllocationStart))
    };

    struct Qv2rayConfig_Connection
    {
        bool bypassCN;
        bool bypassBT;
        bool enableProxy;
        bool v2rayFreedomDNS;
        bool withLocalDNS;
        Qv2rayConfig_Routing routeConfig;
        QList<QString> dnsList;
        Qv2rayConfig_ForwardProxy forwardProxyConfig;
        Qv2rayConfig_Connection()
            : bypassCN(true), bypassBT(false), enableProxy(true), v2rayFreedomDNS(false), withLocalDNS(false), routeConfig(),
              dnsList(QStringList{ "8.8.4.4", "1.1.1.1" }){};
        JSONSTRUCT_REGISTER(Qv2rayConfig_Connection,
                            F(bypassCN, bypassBT, enableProxy, v2rayFreedomDNS, withLocalDNS, dnsList, forwardProxyConfig, routeConfig))
    };
    struct Qv2rayConfig_Kernel
    {
        bool enableAPI;
        int statsPort;
        //
        QString v2CorePath_linux;
        QString v2AssetsPath_linux;
        QString v2CorePath_macx;
        QString v2AssetsPath_macx;
        QString v2CorePath_win;
        QString v2AssetsPath_win;
        Qv2rayConfig_Kernel()
            : enableAPI(true), statsPort(15490),        //
              v2CorePath_linux(), v2AssetsPath_linux(), //
              v2CorePath_macx(), v2AssetsPath_macx(),   //
              v2CorePath_win(), v2AssetsPath_win()      //
              {};
        //
#ifdef Q_OS_LINUX
    #define _VARNAME_VCOREPATH_ v2CorePath_linux
    #define _VARNAME_VASSETSPATH_ v2AssetsPath_linux
#elif defined(Q_OS_MACOS)
    #define _VARNAME_VCOREPATH_ v2CorePath_macx
    #define _VARNAME_VASSETSPATH_ v2AssetsPath_macx
#elif defined(Q_OS_WIN)
    #define _VARNAME_VCOREPATH_ v2CorePath_win
    #define _VARNAME_VASSETSPATH_ v2AssetsPath_win
#endif

        inline const QString KernelPath(const QString &path = "")
        {
            return path.isEmpty() ? _VARNAME_VCOREPATH_ : _VARNAME_VCOREPATH_ = path;
        }
        inline const QString AssetsPath(const QString &path = "")
        {
            return path.isEmpty() ? _VARNAME_VASSETSPATH_ : _VARNAME_VASSETSPATH_ = path;
        }

#undef _VARNAME_VCOREPATH_
#undef _VARNAME_VASSETSPATH_

        JSONSTRUCT_REGISTER(Qv2rayConfig_Kernel,                     //
                            F(enableAPI, statsPort),                 //
                            F(v2CorePath_linux, v2AssetsPath_linux), //
                            F(v2CorePath_macx, v2AssetsPath_macx),   //
                            F(v2CorePath_win, v2AssetsPath_win))
    };

    struct Qv2rayConfig_Update
    {
        QString ignoredVersion;
        ///
        /// \brief updateChannel
        /// 0: Stable
        /// 1: Testing
        int updateChannel;
        JSONSTRUCT_REGISTER(Qv2rayConfig_Update, F(ignoredVersion, updateChannel))
    };

    struct Qv2rayConfig_Advanced
    {
        bool setAllowInsecure;
        bool setAllowInsecureCiphers;
        bool testLatencyPeriodcally;
        JSONSTRUCT_REGISTER(Qv2rayConfig_Advanced, F(setAllowInsecure, setAllowInsecureCiphers, testLatencyPeriodcally))
    };

    struct Qv2rayConfig_Network
    {
        enum Qv2rayProxyType : int
        {
            QVPROXY_NONE = 0,
            QVPROXY_SYSTEM = 1,
            QVPROXY_CUSTOM = 2
        } proxyType;

        QString address;
        QString type;
        int port;
        QString userAgent;
        Qv2rayConfig_Network()
            : proxyType(QVPROXY_NONE), //
              address("127.0.0.1"),    //
              type("http"),            //
              port(8000),              //
              userAgent("Qv2ray/$VERSION WebRequestHelper"){};
        JSONSTRUCT_REGISTER(Qv2rayConfig_Network, F(proxyType, type, address, port, userAgent))
    };

    enum Qv2rayAutoConnectionBehavior
    {
        AUTO_CONNECTION_NONE = 0,
        AUTO_CONNECTION_FIXED = 1,
        AUTO_CONNECTION_LAST_CONNECTED = 2
    };

    struct Qv2rayConfigObject
    {
        int config_version;
        bool tProxySupport;
        int logLevel;
        //
        ConnectionGroupPair autoStartId;
        Qv2rayAutoConnectionBehavior autoStartBehavior;
        //
        // Key = groupId, connectionId
        //        QList<GroupId> groups;
        //        QList<ConnectionId> connections;
        //
        Qv2rayConfig_UI uiConfig;
        Qv2rayConfig_Plugin pluginConfig;
        Qv2rayConfig_Kernel kernelConfig;
        Qv2rayConfig_Update updateConfig;
        Qv2rayConfig_Network networkConfig;
        Qv2rayConfig_Inbounds inboundConfig;
        Qv2rayConfig_Outbounds outboundConfig;
        Qv2rayConfig_Advanced advancedConfig;
        Qv2rayConfig_Connection connectionConfig;

        Qv2rayConfigObject()
            : config_version(QV2RAY_CONFIG_VERSION), //
              tProxySupport(false),                  //
              logLevel(),                            //
              autoStartId(),                         //
              autoStartBehavior(),                   //
              uiConfig(),                            //
              pluginConfig(),                        //
              kernelConfig(),                        //
              updateConfig(),                        //
              networkConfig(),                       //
              inboundConfig(),                       //
              outboundConfig(),                      //
              advancedConfig(),                      //
              connectionConfig(){};

        JSONSTRUCT_REGISTER(Qv2rayConfigObject,                                                                   //
                            F(config_version, tProxySupport, autoStartId, autoStartBehavior, logLevel),           //
                            F(uiConfig, advancedConfig, pluginConfig, updateConfig, kernelConfig, networkConfig), //
                            F(inboundConfig, outboundConfig, connectionConfig))
    };
} // namespace Qv2ray::base::config
