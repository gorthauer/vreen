/****************************************************************************
**
** VKit - vk.com API Qt bindings
**
** Copyright © 1024 + 4096 + 8192 //FIXME в нормальный вид привести
**
*****************************************************************************
**
** $VKIT_BEGIN_LICENSE$
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
** $VKIT_END_LICENSE$
**
****************************************************************************/
#include "directconnection_p.h"
#include "json.h"
#include "reply.h"

#include <QUrl>
#include <QNetworkReply>
#include <QCryptographicHash>
#include <QStringList>
#include <QStringBuilder>

#include <QDebug>


namespace vk {

//TODO use normal framework
struct LoginVars {
    const char *grantType;
    const char *clientId;
    const char *clientSecret;
    const char *appName;
    int scope;
};

static const LoginVars loginVars = {
    "password",
    "1950109",
    "bJKfYSu0LS6N52M0HnBo",
    "TitanIM",
    2 + 4 + 8 + 16 + 1024 + 4096 + 8192 //FIXME в нормальный вид привести

};

struct ServerUrls {
    QUrl apiServer;
    QUrl authServer;
};

static const ServerUrls serverUrls = {
    QUrl(QLatin1String("https://api.vk.com/method/")),
    QUrl(QLatin1String("https://api.vk.com/oauth/token"))
};

static QByteArray paranoicEscape(const QByteArray &raw)
{
    static char hex[17] = "0123456789ABCDEF";
    QByteArray escaped;
    escaped.reserve(raw.size() * 3);
    for (int i = 0; i < raw.size(); ++i) {
        const quint8 c = static_cast<quint8>(raw[i]);
        escaped += '%';
        escaped += hex[c >> 4];
        escaped += hex[c & 0x0f];
    }
    return escaped;
}

DirectConnection::DirectConnection(QObject *parent) :
    Connection(parent),
    m_connectionState(Client::StateOffline)
{
}

void DirectConnection::connectToHost(const QString &login, const QString &password)
{
    setConnectionState(Client::StateConnecting);

    //TODO add token check
    getToken(login, password);
}

void DirectConnection::disconnectFromHost()
{
    setConnectionState(Client::StateOffline);
}

QNetworkReply *DirectConnection::request(const QString &method, const QVariantMap &args)
{
    return get(method, args);
}

Client::State DirectConnection::connectionState() const
{
    return m_connectionState;
}

int DirectConnection::uid() const
{
    return m_token.uid;
}

void DirectConnection::clear()
{
    m_token.accessToken.clear();
    m_token.expireTime = 0;
    m_token.uid = 0;
}

QNetworkReply *DirectConnection::get(const QString &method, const QVariantMap &args)
{
    QUrl url = serverUrls.apiServer;
    url.setPath(url.path() % QLatin1Literal("/") % method);
    auto it = args.constBegin();
    for (; it != args.constEnd(); it++)
        url.addQueryItem(it.key(), it.value().toString());
    url.addEncodedQueryItem("access_token", m_token.accessToken);

    QNetworkRequest request(url);
    QNetworkReply *reply = QNetworkAccessManager::get(request);
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(onReplyError(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(finished()), this, SLOT(onReplyFinished()));
    return reply;
}

void DirectConnection::setConnectionState(Client::State state)
{
    if (m_connectionState != state) {
        m_connectionState = state;
        emit connectionStateChanged(state);
    }
}

void DirectConnection::getToken(const QString &login, const QString &password)
{
    QUrl url = serverUrls.authServer;
    url.addEncodedQueryItem("grant_type", loginVars.grantType);
    url.addEncodedQueryItem("client_id", loginVars.clientId);
    url.addEncodedQueryItem("client_secret", loginVars.clientSecret);
    url.addEncodedQueryItem("scope", QByteArray::number(loginVars.scope));

    url.addQueryItem(QLatin1String("username"), login); //TODO use C++11 literals in suffix
    url.addEncodedQueryItem("password", paranoicEscape(password.toUtf8()));

    QNetworkReply *reply = get(QNetworkRequest(url));
    connect(reply, SIGNAL(finished()), SLOT(getTokenFinished()));
}

void DirectConnection::getTokenFinished()
{
    auto reply = static_cast<QNetworkReply*>(sender());
    reply->deleteLater();

    switch (reply->error()) {
    case QNetworkReply::NoError: {
        QVariantMap response = JSON::parse(reply->readAll()).toMap();
        if (!response.value("access_token").isNull()) {
            m_token.accessToken = response.value("access_token").toByteArray();
            m_token.expireTime = response.value("expires_in").toInt();
            m_token.uid = response.value("user_id").toInt();
            setConnectionState(Client::StateOnline);
            return;
        }
        break;
    }
    case QNetworkReply::AuthenticationRequiredError: //TODO
    default:
        disconnectFromHost();
        emit error(Client::ErrorAuthorizationFailed);
        break;
    }
}

void DirectConnection::onReplyFinished()
{
    //TODO
}

void DirectConnection::onReplyError(QNetworkReply::NetworkError error)
{
    //TODO
}

} // namespace vk

