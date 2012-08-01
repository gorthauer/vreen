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
#include "chatsession.h"
#include "messagesession_p.h"
#include "contact.h"
#include "client.h"
#include "longpoll.h"
#include "utils.h"
#include <QStringBuilder>

namespace vk {

class ChatSessionPrivate : public MessageSessionPrivate
{
    Q_DECLARE_PUBLIC(ChatSession)
public:
    ChatSessionPrivate(ChatSession *q, Contact *contact) :
        MessageSessionPrivate(q, contact->client(), contact->id()),
        contact(contact), isActive(false) {}

    Contact *contact;
    bool isActive;

    void _q_history_received(const QVariant &);
    void _q_message_read_state_updated(const QVariant &);
    void _q_message_added(const Message &message);
};


/*!
 * \brief The ChatSession class
 * Api reference: \link http://vk.com/developers.php?oid=-1&p=Расширенные_методы_API
 */

/*!
 * \brief ChatSession::ChatSession
 * \param contact
 */
ChatSession::ChatSession(Contact *contact) :
    MessageSession(new ChatSessionPrivate(this, contact))
{
    Q_D(ChatSession);
    auto longPoll = d->contact->client()->longPoll();
    connect(longPoll, SIGNAL(messageAdded(vk::Message)),
            this, SLOT(_q_message_added(vk::Message)));
    connect(longPoll, SIGNAL(messageDeleted(int)),
            this, SIGNAL(messageDeleted(int)));
}

ChatSession::~ChatSession()
{

}


Contact *ChatSession::contact() const
{
    return d_func()->contact;
}

void ChatSession::markMessagesAsRead(QList<int> ids, bool set)
{
    Q_D(ChatSession);
    QString request = set ? "messages.markAsRead"
                          : "messages.markAsNew";
    QVariantMap args;
    args.insert("mids", join(ids));
    auto reply = d->contact->client()->request(request, args);
    reply->setProperty("mids", qVariantFromValue(ids));
    reply->setProperty("set", set);

}

QString ChatSession::title() const
{
    return contact()->name();
}

bool ChatSession::isActive() const
{
    return d_func()->isActive;
}

void ChatSession::setActive(bool set)
{
    Q_D(ChatSession);
    d->isActive = set;
}

Reply *ChatSession::doGetHistory(int count, int offset)
{
    Q_D(ChatSession);
    QVariantMap args;
    args.insert("count", count);
    args.insert("offset", offset);
    args.insert("uid", d->contact->id());

    auto reply = d->contact->client()->request("messages.getHistory", args);
    connect(reply, SIGNAL(resultReady(QVariant)), SLOT(_q_history_received(QVariant)));
    return reply;
}

Reply *ChatSession::doSendMessage(const Message &message)
{
    Q_D(ChatSession);
    return d->contact->client()->sendMessage(message);
}

void ChatSessionPrivate::_q_history_received(const QVariant &response)
{
    auto list = response.toList();
    Q_UNUSED(list.takeFirst());
    foreach (auto item, list) {
        QVariantMap map = item.toMap();
        Message message(map, contact->client());
        emit q_func()->messageAdded(message);
    }
}

void ChatSessionPrivate::_q_message_read_state_updated(const QVariant &response)
{
    Q_Q(ChatSession);
    auto reply = qobject_cast<Reply*>(q->sender());
    if (response.toInt() == 1) {
        auto set = reply->property("set").toBool();
        auto ids = reply->property("mids").value<IdList>();
        foreach(int id, ids)
            emit q->messageReadStateChanged(id, set);
    }
}

void ChatSessionPrivate::_q_message_added(const Message &message)
{
    auto sender = message.isIncoming() ? message.from() : message.to();
    if (sender == contact || !sender) //HACK some workaround
        emit q_func()->messageAdded(message);
}

} // namespace vk

#include "moc_chatsession.cpp"

