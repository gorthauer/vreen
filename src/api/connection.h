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
#ifndef VK_CONNECTION_H
#define VK_CONNECTION_H

#include <QNetworkAccessManager>
#include <QVariantMap>
#include "client.h"

namespace vk {

class Reply;
class VK_SHARED_EXPORT Connection : public QNetworkAccessManager
{
    Q_OBJECT
public:
    Connection(QObject *parent = 0);

    virtual void connectToHost(const QString &login, const QString &password) = 0;
    virtual void disconnectFromHost() = 0;
    virtual QNetworkReply *request(const QString &method, const QVariantMap &args = QVariantMap()) = 0;
    virtual Client::State connectionState() const = 0;
    virtual int uid() const = 0;
    virtual void clear();
signals:
    void connectionStateChanged(vk::Client::State connectionState);
    void error(vk::Client::Error);
};

} //namespace vk

#endif // VK_CONNECTION_H

