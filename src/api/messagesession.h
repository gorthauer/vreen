#ifndef VK_MESSAGESESSION_H
#define VK_MESSAGESESSION_H

#include <QObject>
#include "message.h"

namespace vk {

class Message;
class Client;
class Reply;
class MessageSessionPrivate;

class VK_SHARED_EXPORT MessageSession : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(MessageSession)

    Q_PROPERTY(int uid READ uid CONSTANT)
    Q_PROPERTY(Client* client READ client CONSTANT)
public:
    explicit MessageSession(MessageSessionPrivate *data);
    virtual ~MessageSession();
    Client *client() const;
    int uid() const;
public slots:
    Reply *getHistory(int count = 16, int offset = 0);
    Reply *sendMessage(const QString &body, const QString &subject = QString());
    Reply *sendMessage(const Message &message);
signals:
    void messageAdded(const vk::Message &message);
    void messageDeleted(int id);
    void messageReadStateChanged(int mid, bool isRead);
protected:
    virtual Reply *doSendMessage(const vk::Message &message) = 0;
    virtual Reply *doGetHistory(int count, int offset) = 0;
    QScopedPointer<MessageSessionPrivate> d_ptr;
};

} // namespace vk

#endif // VK_MESSAGESESSION_H
