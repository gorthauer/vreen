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
#ifndef VK_CHATSESSION_H
#define VK_CHATSESSION_H

#include "message.h"
#include "messagesession.h"

namespace vk {

class Reply;
class ChatSessionPrivate;
class VK_SHARED_EXPORT ChatSession : public MessageSession
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(ChatSession)

    Q_PROPERTY(Contact *contact READ contact CONSTANT)
public:
    ChatSession(Contact *contact);
    virtual ~ChatSession();

    Contact *contact() const;
    void markMessagesAsRead(IdList ids, bool set = true);
    QString title() const;
    bool isActive() const;
    void setActive(bool set);
signals:
    void messageAdded(const vk::Message &message);
    void messageDeleted(int id);
    void messageReadStateChanged(int mid, bool isRead);
protected:
    virtual Reply *doGetHistory(int count = 16, int offset = 0);
    virtual Reply *doSendMessage(const vk::Message &message);
private:

    Q_PRIVATE_SLOT(d_func(), void _q_history_received(const QVariant &))
    Q_PRIVATE_SLOT(d_func(), void _q_message_added(const vk::Message &))
};

} // namespace vk

#endif // VK_CHATSESSION_H

