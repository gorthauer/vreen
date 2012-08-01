#ifndef VK_GROUPCHATSESSION_H
#define VK_GROUPCHATSESSION_H
#include "messagesession.h"
#include "contact.h"

namespace vk {

class GroupChatSessionPrivate;

class VK_SHARED_EXPORT GroupChatSession : public vk::MessageSession
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(GroupChatSession)

    Q_PROPERTY(QString title READ title NOTIFY titleChanged)
public:
    explicit GroupChatSession(int chatId, Client *parent);

    BuddyList participants() const;
    Buddy *admin() const;
    QString title() const;
    Buddy *findParticipant(int uid) const;

    static Reply *create(Client *client, const IdList &uids, const QString &title = QString());
public slots:
    Reply *getInfo();
    Reply *addParticipant(Contact *buddy);
    Reply *removeParticipant(Contact *buddy);
    Reply *updateTitle(const QString &title);
signals:
    void participantAdded(vk::Buddy*);
    void participantRemoved(vk::Buddy*);
    void titleChanged(QString);
protected:
    void setTitle(const QString &title);
    virtual Reply *doSendMessage(const vk::Message &message);
    virtual Reply *doGetHistory(int count = 16, int offset = 0);
private:
    Q_PRIVATE_SLOT(d_func(), void _q_history_received(const QVariant &response))
    Q_PRIVATE_SLOT(d_func(), void _q_info_received(const QVariant &response))
    Q_PRIVATE_SLOT(d_func(), void _q_participant_added(const QVariant &response))
    Q_PRIVATE_SLOT(d_func(), void _q_participant_removed(const QVariant &response))
    Q_PRIVATE_SLOT(d_func(), void _q_title_updated(const QVariant &response))
    Q_PRIVATE_SLOT(d_func(), void _q_online_changed(bool))
};

} // namespace vk

#endif // VK_GROUPCHATSESSION_H
