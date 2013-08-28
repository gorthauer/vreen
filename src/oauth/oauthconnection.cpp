/****************************************************************************
**
** Vreen - vk.com API Qt bindings
**
** Copyright © 2012 Aleksey Sidorov <gorthauer87@ya.ru>
**
*****************************************************************************
**
** $VREEN_BEGIN_LICENSE$
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
** See the GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see http://www.gnu.org/licenses/.
** $VREEN_END_LICENSE$
**
****************************************************************************/
#include "oauthconnection.h"
#include <QWebPage>
#include <QWebFrame>
#include <QPointer>
#include <QDateTime>
#include <QStringBuilder>
#include <QNetworkRequest>
#include <QTextDocument>

#include <json.h>
#include <vreen/private/connection_p.h>
#include <vreen/utils.h>

#include <QDebug>
#include <QWebElement>
#include <QWebView>
#include <QCoreApplication>

#include <QSettings>

namespace Vreen {

const static QUrl authUrl("http://api.vk.com/oauth/authorize");
const static QUrl apiUrl("https://api.vk.com/method/");
const static char *scopeNames[] = { "notify", "friends", "photos", "audio", 
    "video", "docs", "notes", "pages", "status", "offers", "questions", "wall", 
    "groups", "messages", "notifications", "stats", "ads", "offline" };

class OAuthConnection;
class OAuthConnectionPrivate : public ConnectionPrivate
{
    Q_DECLARE_PUBLIC(OAuthConnection)
public:
    OAuthConnectionPrivate(OAuthConnection *q, int appId) : ConnectionPrivate(q),
        connectionState(Client::StateOffline),
        clientId(appId),
        scope(OAuthConnection::Notify|
              OAuthConnection::Friends|
              OAuthConnection::Photos|
              OAuthConnection::Audio|
              OAuthConnection::Video|
              OAuthConnection::Docs|
              OAuthConnection::Notes|
              OAuthConnection::Pages|
              OAuthConnection::Status|
              OAuthConnection::Offers|
              OAuthConnection::Questions|
              OAuthConnection::Wall|
              OAuthConnection::Groups|
              OAuthConnection::Messages|
              OAuthConnection::Notifications|
              OAuthConnection::Stats|
              OAuthConnection::Ads|
              OAuthConnection::Offline),
        redirectUri(QLatin1String("http://oauth.vk.com/blank.html")),
        displayType(OAuthConnection::Touch),
        responseType(QLatin1String("token")),
        uid(0),
        expiresIn(0)
    {
    }
    QPointer<QWebPage> webPage;
    QPointer<QWebView> webView;
    Client::State connectionState;

    //OAuth settings
    int clientId;
    OAuthConnection::Scopes scope; //settings
    QString redirectUri;
    OAuthConnection::DisplayType displayType;
    QString responseType;

    QString login;
    QString password;

    //response
    QByteArray accessToken;
    int uid;
    time_t expiresIn;

    void requestToken();
    void setConnectionState(Client::State state);
    void _q_loadFinished(bool);
    void clear();
    void handleAuthRequest(QWebPage *page);
    void saveAuthData();
    void loadAuthData();
};


OAuthConnection::OAuthConnection(int appId, QObject *parent) :
    Connection(new OAuthConnectionPrivate(this, appId), parent)
{
}

OAuthConnection::OAuthConnection(QObject *parent) :
    Connection(new OAuthConnectionPrivate(this, -1), parent)
{
}

OAuthConnection::~OAuthConnection()
{
}

void OAuthConnection::connectToHost(const QString &login, const QString &password)
{
    Q_D(OAuthConnection);
    if (d->options.value(Connection::KeepAuthData).toBool())
        d->loadAuthData();

    if (d->login != login || d->password != password) {
        if (!(d->login.isNull() || d->password.isNull()))
            d->clear();
        d->login = login;
        d->password = password;
    }
    if (!d->uid || d->accessToken.isNull()
            || (d->expiresIn && d->expiresIn < static_cast<time_t>(QDateTime::currentDateTime().toTime_t()))) {
        d->requestToken();
        d->setConnectionState(Client::StateConnecting);
    } else
        d->setConnectionState(Client::StateOnline);
}

void OAuthConnection::disconnectFromHost()
{
    Q_D(OAuthConnection);
    d->setConnectionState(Client::StateOffline);
}

QNetworkRequest OAuthConnection::makeRequest(const QString &method, const QVariantMap &args)
{
    Q_D(OAuthConnection);

    //TODO test expiresIn
    QUrl url = apiUrl;
    url.setPath(url.path() % QLatin1Literal("/") % method);
    QVariantMap::const_iterator it = args.constBegin();
    for (; it != args.constEnd(); it++)
        url.addEncodedQueryItem(QUrl::toPercentEncoding(it.key()),
                                QUrl::toPercentEncoding(it.value().toString()));
        //url.addQueryItem(it.key(), toHtmlEscaped(it.value().toString()));
    url.addEncodedQueryItem("access_token", d->accessToken);

    QNetworkRequest request(url);
    return request;
}

void OAuthConnection::decorateRequest(QNetworkRequest &request)
{
    Q_D(OAuthConnection);
    auto url = request.url();
    url.addEncodedQueryItem("access_token", d->accessToken);
    request.setUrl(url);
}

Client::State OAuthConnection::connectionState() const
{
    return d_func()->connectionState;
}

int OAuthConnection::uid() const
{
    return d_func()->uid;
}

void OAuthConnection::clear()
{
    d_func()->clear();
}

QByteArray OAuthConnection::accessToken() const
{
    return d_func()->accessToken;
}

time_t OAuthConnection::expiresIn() const
{
    return d_func()->expiresIn;
}

void OAuthConnection::setAccessToken(const QByteArray &token, time_t expiresIn)
{
    Q_D(OAuthConnection);
    if (!(d->accessToken == token && d->expiresIn == expiresIn)) {
        d->accessToken = token;
        d->expiresIn = expiresIn;
        emit accessTokenChanged(token, expiresIn);
    }
}

void OAuthConnection::setUid(int uid)
{
    d_func()->uid = uid;
}

int OAuthConnection::clientId() const
{
    return d_func()->clientId;
}

void OAuthConnection::setClientId(int clientId)
{
    Q_D(OAuthConnection);
    if (d->clientId != clientId) {
        d->clientId = clientId;
        emit clientIdChanged(clientId);
    }
}

OAuthConnection::DisplayType OAuthConnection::displayType() const
{
    return d_func()->displayType;
}

void OAuthConnection::setDisplayType(OAuthConnection::DisplayType displayType)
{
    Q_D(OAuthConnection);
    if (d->displayType != displayType) {
        d->displayType = displayType;
    }
}

OAuthConnection::Scopes OAuthConnection::scopes() const
{
    return d_func()->scope;
}

void OAuthConnection::setScopes(OAuthConnection::Scopes scopes)
{
    Q_D(OAuthConnection);
    if( d->scope != scopes ) {
        d->scope = scopes;
        emit scopesChanged(scopes);
    }
}

void OAuthConnectionPrivate::requestToken()
{
    Q_Q(OAuthConnection);
    if (!webPage) {
        webPage = new QWebPage(q);
        webPage->setNetworkAccessManager(q);
        q->connect(webPage->mainFrame(), SIGNAL(loadFinished(bool)), SLOT(_q_loadFinished(bool)));
    }
    QUrl url = authUrl;
    url.addQueryItem(QLatin1String("client_id"), QByteArray::number(clientId));
    url.addQueryItem(QLatin1String("scope"), flagsToStrList(scope, scopeNames).join(","));
    url.addQueryItem(QLatin1String("redirect_uri"), redirectUri);
    const char *type[] = {
        "page",
        "popup",
        "touch",
        "wap"
    };
    url.addQueryItem(QLatin1String("display"), type[displayType]);
    url.addQueryItem(QLatin1String("response_type"), responseType);
    webPage->mainFrame()->load(url);
}

void OAuthConnectionPrivate::setConnectionState(Client::State state)
{
    Q_Q(OAuthConnection);
    if (state != connectionState) {
        connectionState = state;
        emit q->connectionStateChanged(state);
    }
}

void OAuthConnectionPrivate::_q_loadFinished(bool ok)
{
    Q_Q(OAuthConnection);
    QUrl url = webPage->mainFrame()->url();
    QVariantMap response = Vreen::JSON::parse(webPage->mainFrame()->toPlainText().toUtf8()).toMap();
    if (ok && response.value("error").isNull()) {
        url = QUrl("http://foo.bar?" + url.fragment()); //evil hack for parse fragment as query items
        if (!url.hasEncodedQueryItem("access_token")) {
            if (!webPage->view()) {
                QWebFrame *frame = webPage->mainFrame();

                QWebElement element = frame->findFirstElement("input[name=email]");
                element.setAttribute("value", login);
                element = frame->findFirstElement("input[name=pass]");
                element.setAttribute("value", password);
                element = frame->findFirstElement("#login_enter");
                element.setFocus();
                handleAuthRequest(webPage.data());
            }
        } else {
            accessToken = url.encodedQueryItemValue("access_token");
            expiresIn = url.encodedQueryItemValue("expires_in").toUInt();
            if (expiresIn)
                expiresIn += QDateTime::currentDateTime().toTime_t(); //not infinity token
            uid = url.encodedQueryItemValue("user_id").toInt();
            emit q->accessTokenChanged(accessToken, expiresIn);

            setConnectionState(Client::StateOnline);
            webPage->deleteLater();
            if (options.value(Connection::KeepAuthData).toBool())
                saveAuthData();
        }
    } else {
        setConnectionState(Client::StateOffline);
        emit q->error(Client::ErrorAuthorizationFailed);
        webPage->deleteLater();
        clear();
    }
}

void OAuthConnectionPrivate::clear()
{
    accessToken.clear();
    expiresIn = 0;
    uid = 0;
    saveAuthData();
}

void OAuthConnectionPrivate::handleAuthRequest(QWebPage *page)
{
    Q_Q(OAuthConnection);
    if (options.value(Connection::ShowAuthDialog).toBool()) {
        if (!webView) {
            webView = new QWebView;
            webView->setWindowModality(Qt::ApplicationModal);
            webView->connect(page, SIGNAL(destroyed()), webView.data(), SLOT(deleteLater()));
            webView->setAttribute(Qt::WA_DeleteOnClose, true);
            webView->setPage(page);
        }
        webView->showNormal();
    } else
        emit q->authConfirmRequested(page);
}

void OAuthConnectionPrivate::saveAuthData()
{
    Q_Q(OAuthConnection);
    QSettings cfg;
    cfg.beginGroup("access");
    cfg.setValue("uid", q->uid());
    cfg.setValue("token", q->accessToken());
    cfg.setValue("expires", qint64(q->expiresIn()));
    cfg.endGroup();
    cfg.sync();
}

void OAuthConnectionPrivate::loadAuthData()
{
    Q_Q(OAuthConnection);
    QSettings cfg;
    cfg.beginGroup("access");
    q->setUid(cfg.value("uid").toInt());
    q->setAccessToken(cfg.value("token").toByteArray(),
                      cfg.value("expires").toLongLong());
    cfg.endGroup();
}

} // namespace Vreen

#include "moc_oauthconnection.cpp"

