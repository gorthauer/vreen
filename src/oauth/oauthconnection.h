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
#ifndef VK_OAUTHCONNECTION_H
#define VK_OAUTHCONNECTION_H

#include <vreen/connection.h>
#include <ctime>

class QWebPage;

namespace Vreen {

class OAuthConnectionPrivate;

class OAuthConnection : public Connection
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(OAuthConnection)
    Q_ENUMS(DisplayType)
    Q_FLAGS(Scopes)
    Q_PROPERTY(int clientId READ clientId WRITE setClientId NOTIFY clientIdChanged)
    Q_PROPERTY(DisplayType displayType READ displayType WRITE setDisplayType)
    Q_PROPERTY(Scopes scopes READ scopes WRITE setScopes NOTIFY scopesChanged)
public:
    enum DisplayType {
        Page,
        Popup,
        Touch,
        Wap
    };
    enum Scope {
        notify        = 0x1,
        friends       = 0x2,
        photos        = 0x4,
        audio         = 0x8,
        video         = 0x10,
        docs          = 0x20,
        notes         = 0x40,
        pages         = 0x80,
        status        = 0x100,
        offers        = 0x200,
        questions     = 0x400,
        wall          = 0x800,
        groups        = 0x1000,
        messages      = 0x2000,
        notifications = 0x4000,
        stats         = 0x8000,
        ads           = 0x10000,
        offline       = 0x20000
    };
    Q_DECLARE_FLAGS(Scopes, Scope)
    
    explicit OAuthConnection(int clientId, QObject *parent = 0);
    explicit OAuthConnection(QObject *parent = 0);
    virtual ~OAuthConnection();

    void connectToHost(const QString &login, const QString &password);
    void disconnectFromHost();
    QNetworkReply *request(const QString &method, const QVariantMap &args = QVariantMap());
    Client::State connectionState() const;
    int uid() const;
    void clear();

    QByteArray accessToken() const;
    time_t expiresIn() const;
    void setAccessToken(const QByteArray &token, time_t expiresIn = 0);
    void setUid(int uid);
    int clientId() const;
    void setClientId(int clientId);
    DisplayType displayType() const;
    void setDisplayType(DisplayType displayType);
    Scopes scopes() const;
    void setScopes(Scopes scopes);
signals:
    void authConfirmRequested(QWebPage *page);
    void accessTokenChanged(const QByteArray &token, time_t expiresIn);
    void clientIdChanged(int clientId);
    void scopesChanged(Vreen::OAuthConnection::Scopes scopes);
protected:
	QNetworkRequest makeRequest(const QString &method, const QVariantMap &args = QVariantMap());
    void decorateRequest(QNetworkRequest &);
private:
    Q_PRIVATE_SLOT(d_func(), void _q_loadFinished(bool))
};

Q_DECLARE_OPERATORS_FOR_FLAGS(OAuthConnection::Scopes)

} // namespace Vreen

#endif // VK_OAUTHCONNECTION_H

