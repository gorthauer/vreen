/****************************************************************************
**
** VKit - vk.com API Qt bindings
**
** Copyright © 2012 Aleksey Sidorov <gorthauer87@ya.ru>
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
#ifndef VK_DIRECTCONNECTION_H
#define VK_DIRECTCONNECTION_H

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include "connection.h"

namespace vk {

class DirectConnection : public Connection
{
    Q_OBJECT
public:
    explicit DirectConnection(QObject *parent = 0);

    virtual void connectToHost(const QString &login, const QString &password);
    virtual void disconnectFromHost();
    virtual QNetworkReply *request(const QString &method, const QVariantMap &args);
    virtual Client::State connectionState() const;
    virtual int uid() const;
    virtual void clear();

    using Connection::get;
    QNetworkReply *get(const QString &method, const QVariantMap &args = QVariantMap());
protected:
    void setConnectionState(Client::State connectionState);
    void getToken(const QString &login, const QString &password);
protected slots:
    void getTokenFinished();
    void onReplyFinished();
    void onReplyError(QNetworkReply::NetworkError error);
private:
    Client::State m_connectionState;
    struct AccessToken {
        AccessToken() {}
        QByteArray accessToken;
        int expireTime;
        int uid;
    };
    AccessToken m_token;
};

} // namespace vk

#endif // VK_DIRECTCONNECTION_H

