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
#include "reply_p.h"
#include <QNetworkReply>

namespace vk {

Reply::Reply(QNetworkReply *reply) :
    QObject(reply),
    d_ptr(new ReplyPrivate(this))
{
    setReply(reply);

    //qDebug() << "--Send reply:" << reply->url();
}

Reply::~Reply()
{
    //FIXME maybee it's never been used
    if (auto networkReply = d_func()->networkReply.data())
        networkReply->deleteLater();
}

QNetworkReply *Reply::networkReply() const
{
    return d_func()->networkReply.data();
}

QVariant Reply::response() const
{
    return d_func()->response;
}

QVariant Reply::error() const
{
    return d_func()->error;
}

void Reply::setReply(QNetworkReply *reply)
{
    Q_D(Reply);    
    if (!d->networkReply.isNull())
        d->networkReply.data()->deleteLater();
    d->networkReply = reply;
    setParent(reply);

    connect(reply, SIGNAL(finished()), SLOT(_q_reply_finished()));
}

} // namespace vk

#include "moc_reply.cpp"

