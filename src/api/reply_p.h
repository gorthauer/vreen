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
#ifndef REPLY_P_H
#define REPLY_P_H
#include "reply.h"
#include "json.h"
#include "reply.h"
#include "QNetworkReply"
#include <QWeakPointer>
#include <QDebug>

namespace vk {

class ReplyPrivate
{
    Q_DECLARE_PUBLIC(Reply)
public:
    ReplyPrivate(Reply *q) : q_ptr(q) {}
    Reply *q_ptr;

    QWeakPointer<QNetworkReply> networkReply;
    QVariant response;
    QVariant error;

    void _q_reply_finished()
    {
        Q_Q(Reply);
        auto reply = static_cast<QNetworkReply*>(q->sender());
        QVariantMap map = JSON::parse(reply->readAll()).toMap();
        //TODO error and captcha handler

        //qDebug() << "--Reply finished: " << reply->url().encodedPath();

        response = map.value("response");
        if (!response.isNull()) {
            emit q->resultReady(response);
            return;
        } else {
            error = map.value("error");
            int errorCode = error.toMap().value("error_code").toInt();
            if (errorCode) {
                emit q->error(errorCode);
                return;
            }
        }
        if (!map.isEmpty()) {
            response = map;
            emit q->resultReady(response);
        }

    }
};


} //namespace vk

#endif // REPLY_P_H

