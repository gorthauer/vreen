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
#include "message.h"
#include "contact.h"
#include "client.h"
#include "roster.h"
#include <QDateTime>
#include "dynamicpropertydata_p.h"
#include <QDebug>

namespace vk {

class MessageData : public DynamicPropertyData
{
public:
    MessageData(Client *client) :
        client(client),
        messageId(0),
        fromId(0),
        toId(0),
        chatId(0),
        userCount(0),
        latitude(-1),
        longitude(-1)
    {}
    MessageData(const MessageData &o) :
        DynamicPropertyData(o),
        client(o.client),
        messageId(o.messageId),
        fromId(o.fromId),
        toId(o.toId),
        date(o.date),
        flags(o.flags),
        subject(o.subject),
        body(o.body),
        forwardMsgIds(o.forwardMsgIds),
        chatId(o.chatId),
        chatActive(o.chatActive),
        userCount(o.userCount),
        admin(o.admin),
        latitude(o.latitude),
        longitude(o.longitude),
        attachmentHash(o.attachmentHash)
    {}
    ~MessageData() {}

    Client *client;
    int messageId;
    int fromId;
    int toId;
    QDateTime date;
    Message::Flags flags;
    QString subject;
    QString body;
    QList<int> forwardMsgIds;
    int chatId;
    QList<int> chatActive;
    int userCount;
    QWeakPointer<Contact> admin;
    qreal latitude;
    qreal longitude;
    Attachment::Hash attachmentHash;

    void fill(const QVariantMap &data)
    {
        messageId = data.value("mid").toInt();

        int clientId = data.value("from_id").toInt();
        if (clientId) {
            auto contact = client->roster()->buddy(clientId);
            bool isIncoming = (contact == client->me());
            setFlag(Message::FlagOutbox, !isIncoming);
            if (isIncoming) {
                fromId = getId(client->me());
                toId = 0;
            } else {
                fromId = getId(contact);
                toId = getId(client->me());
            }
        } else {
            setFlag(Message::FlagOutbox, data.value("out").toBool());
            clientId = data.value("uid").toInt();
            auto contact = client->roster()->buddy(clientId);
            if (!flags.testFlag(Message::FlagOutbox)) {
                fromId = getId(contact);
                toId = getId(client->me());
            } else {
                toId = getId(contact);
                fromId = getId(client->me());
            }
        }

        date = QDateTime::fromTime_t(data.value("date").toInt());
        setFlag(Message::FlagUnread, !data.value("read_state").toBool());
        subject = data.value("title").toString();
        body = data.value("body").toString();
        attachmentHash = Attachment::toHash(Attachment::fromVariantList(data.value("attachments").toList()));
        //TODO forward messages
        //TODO groupchats
    }

    void setFlag(Message::Flag flag, bool set = true)
    {
        if (set)
            flags |= flag;
        else
            flags &= ~flag;
    }

    int getId(Contact *contact) const
    {
        return contact ? contact->id() : 0;
    }
    Contact *getContact(int id) const
    {
        return client ? client->contact(id) : 0;
    }
};


/*!
 * \brief The Message class
 * Api reference: \link http://vk.com/developers.php?oid=-1&p=Формат_описания_личных_сообщений */

Message::Message(Client *client) :
    d(new MessageData(client))
{
}

Message::Message(const QVariantMap &data, Client *client) :
    d(new MessageData(client))
{
    d->fill(data);
}

Message::Message(const Message &other) : d(other.d)
{
}

Message &Message::operator =(const Message &other)
{
    if (this != &other)
        d.operator=(other.d);
    return *this;
}

bool Message::operator ==(const Message &other)
{
    return id() == other.id();
}

Message::~Message()
{
}

int Message::id() const
{
    return d->messageId;
}

void Message::setId(int id)
{
    d->messageId = id;
}

Client *Message::client() const
{
    return d->client;
}

QDateTime Message::date() const
{
    return d->date;
}

void Message::setDate(const QDateTime &date)
{
    d->date = date;
}

int Message::fromId() const
{
    return d->fromId;
}

void Message::setFromId(int id)
{
    d->fromId = id;
}

int Message::toId() const
{
    return d->toId;
}

void Message::setToId(int id)
{
    d->toId = id;
}

Contact *Message::from() const
{
    return d->getContact(d->fromId);
}

void Message::setFrom(Contact *contact)
{
    d->fromId = contact->id();
}

Contact *Message::to() const
{
    return d->getContact(d->toId);
}

void Message::setTo(Contact *to)
{
    d->toId = to->id();
}

QString Message::subject() const
{
    return d->subject;
}

void Message::setSubject(const QString &title)
{
    d->subject = title;
}

QString Message::body() const
{
    return d->body;
}

void Message::setBody(const QString &body)
{
    d->body = body;
}

bool Message::isUnread() const
{
    return testFlag(FlagUnread);
}

void Message::setUnread(bool set)
{
    setFlag(FlagUnread, set);
}

bool Message::isIncoming() const
{
    return !testFlag(FlagOutbox);
}

void Message::setIncoming(bool set)
{
    setFlag(FlagOutbox, !set);
}

void Message::setFlags(Message::Flags flags)
{
    d->flags = flags;
}

Message::Flags Message::flags() const
{
    return d->flags;
}

void Message::setFlag(Flag flag, bool set)
{
    d->setFlag(flag, set);
}

bool Message::testFlag(Flag flag) const
{
    return d->flags.testFlag(flag);
}

Attachment::Hash Message::attachments() const
{
    return d->attachmentHash;
}

Attachment::List Message::attachments(Attachment::Type type) const
{
    return d->attachmentHash.values(type);
}

void Message::setAttachments(const Attachment::List &attachmentList)
{
    d->attachmentHash = Attachment::toHash(attachmentList);
}

MessageList Message::fromVariantList(const QVariantList &list, vk::Client *client)
{
    MessageList messageList;
    foreach (auto item, list) {
        vk::Message message(item.toMap(), client);
        messageList.append(message);
    }
    return messageList;
}

} // namespace vk

