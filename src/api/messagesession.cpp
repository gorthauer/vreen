#include "messagesession_p.h"
#include "client.h"

namespace vk {

MessageSession::MessageSession(MessageSessionPrivate *data) :
    QObject(data->client),
    d_ptr(data)
{
}
MessageSession::~MessageSession()
{
}

Client *MessageSession::client() const
{
    return d_func()->client;
}

int MessageSession::uid() const
{
    return d_func()->uid;
}

Reply *MessageSession::getHistory(int count, int offset)
{
    return doGetHistory(count, offset);
}

Reply *MessageSession::sendMessage(const QString &body, const QString &subject)
{
    Q_D(MessageSession);
    Message msg(d->client);
    msg.setToId(d->uid);
    msg.setBody(body);
    msg.setSubject(subject);
    return sendMessage(msg);
}

Reply *MessageSession::sendMessage(const Message &message)
{
    return doSendMessage(message);
}

} // namespace vk

#include "moc_messagesession.cpp"
