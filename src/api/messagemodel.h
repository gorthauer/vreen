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
#ifndef MESSAGEMODEL_H
#define MESSAGEMODEL_H

#include <QAbstractListModel>
#include "message.h"

namespace vk {

class MessageListModelPrivate;
class VK_SHARED_EXPORT MessageListModel : public QAbstractListModel
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(MessageListModel)
    Q_PROPERTY(Qt::SortOrder sortOrder READ sortOrder WRITE setSortOrder NOTIFY sortOrderChanged)
public:

    enum Roles {
        SubjectRole = Qt::UserRole + 1,
        BodyRole,
        FromRole,
        ToRole,
        ReadStateRole,
        DirectionRole,
        DateRole,
        IdRole,
        AttachmentRole
    };

    MessageListModel(QObject *parent = 0);
    virtual ~MessageListModel();
    int count() const;
    Message at(int index) const;
    int findMessage(int id);
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    virtual int rowCount(const QModelIndex &parent) const;
    void setSortOrder(Qt::SortOrder order);
    Qt::SortOrder sortOrder() const;
signals:
    void sortOrderChanged(Qt::SortOrder order);
public slots:
    void addMessage(const vk::Message &message);
    void removeMessage(const vk::Message &message);
    void removeMessage(int id);
    void setMessages(const vk::MessageList &messages);
    void clear();
protected:
    virtual void doReplaceMessage(int index, const::vk::Message &message);
    virtual void doInsertMessage(int index, const::vk::Message &message);
    virtual void doRemoveMessage(int index);
    virtual void sort(int column, Qt::SortOrder order);
protected slots:
    void replaceMessageFlags(int id, int mask, int userId = 0);
    void resetMessageFlags(int id, int mask, int userId = 0);
private:
    QScopedPointer<MessageListModelPrivate> d_ptr;
};

} //namespace vk

#endif // MESSAGEMODEL_H

