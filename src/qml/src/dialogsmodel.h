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
#ifndef DIALOGSMODEL_H
#define DIALOGSMODEL_H
#include <messagemodel.h>
#include <roster.h>
#include <QWeakPointer>

class DialogsModel : public vk::MessageListModel
{
    Q_OBJECT

    Q_PROPERTY(QObject* client READ client WRITE setClient NOTIFY clientChanged)
    Q_PROPERTY(int unreadCount READ unreadCount NOTIFY unreadCountChanged)
public:
    explicit DialogsModel(QObject *parent = 0);

    //HACK workaround about "Unable to assign QObject* to void"
    void setClient(QObject *client);
    //void setClient(vk::Client *client);
    //vk::Client *client() const;
    QObject *client() const;

    void setUnreadCount(int count);
    int unreadCount() const;
public slots:
    void getDialogs(int offset = 0, int count = 25, int previewLength = -1);
signals:
    //void clientChanged(vk::Client*);
    void clientChanged(QObject*); //HACK
    void unreadCountChanged(int count);
    void requestFinished();
protected:
    virtual void doReplaceMessage(int index, const::vk::Message &message);
    virtual void doInsertMessage(int index, const::vk::Message &message);
    virtual void doRemoveMessage(int index);
private slots:
    void onDialogsReceived(const QVariant &dialogs);
    void onAddMessage(const vk::Message &message);
private:
    QWeakPointer<vk::Client> m_client;
    int m_unreadCount;
};

#endif // DIALOGSMODEL_H

