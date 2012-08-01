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
#include "commentsmodel.h"
#include <contact.h>
#include <roster.h>
#include <client.h>
#include <QDateTime>
#include <QApplication>
#include <QNetworkReply>

#include <QDebug>

CommentsModel::CommentsModel(QObject *parent) :
    QAbstractListModel(parent),
    m_postId(0)
{
    auto roles = roleNames();
    roles[BodyRole] = "body";
    roles[FromRole] = "from";
    roles[DateRole] = "date";
    roles[IdRole] = "id";
    roles[LikesRole] = "likes";
    setRoleNames(roles);
}

vk::Contact *CommentsModel::contact() const
{
    return m_contact.data();
}

void CommentsModel::setContact(vk::Contact *contact)
{
    if (contact == m_contact.data())
        return;
    if (!m_session.isNull()) {
        clear();
        m_session.data()->deleteLater();
    }
    if (!contact)
        return;
    auto session = new vk::CommentSession(contact);
    connect(session, SIGNAL(commentAdded(QVariantMap)), SLOT(addComment(QVariantMap)));
    connect(session, SIGNAL(commentDeleted(int)), SLOT(deleteComment(int)));
    session->setPostId(m_postId);

    m_contact = contact;
    m_session = session;
    emit contactChanged(contact);
}

QVariant CommentsModel::data(const QModelIndex &index, int role) const
{
    int row = index.row();

    auto comment = m_comments.at(row);
    auto roster = m_contact.data()->client()->roster();
    switch (role) {
    case IdRole:
        return comment.value("cid");
        break;
    case FromRole: {
        int source = comment.value("uid").toInt();
        return qVariantFromValue(roster->buddy(source));
    }
    case DateRole:
        return QDateTime::fromTime_t(comment.value("date").toUInt());
    case BodyRole:
        return comment.value("text");
    default:
        break;
    }
    return QVariant::Invalid;
}

int CommentsModel::rowCount(const QModelIndex &) const
{
    return m_comments.count();
}

int CommentsModel::count() const
{
    return m_comments.count();
}

int CommentsModel::findComment(int id) const
{
    for (int i = 0; i != m_comments.count(); i++) {
        if (data(createIndex(i, 0), IdRole).toInt() == id)
            return i;
    }
    return -1;
}

int CommentsModel::postId() const
{
    return m_postId;
}

void CommentsModel::setPostId(int postId)
{
    if (m_postId != postId) {
        m_postId = postId;
        emit postChangedId(postId);
        if (m_session.data())
            m_session.data()->setPostId(postId);
    }
}

void CommentsModel::deleteComment(int id)
{
    int index = findComment(id);
    if (index == -1)
        return;
    beginRemoveRows(QModelIndex(), index, index);
    m_comments.removeAt(index);
    endRemoveRows();
}


static bool commentLessThan(const QVariantMap &a, const QVariantMap &b)
{
    return a.value("cid").toInt() < b.value("cid").toInt();
}

void CommentsModel::addComment(const QVariantMap &data)
{
    int cid = data.value("cid").toInt();
    if (findComment(cid) != -1)
        return;

    auto it = qLowerBound(m_comments.begin(), m_comments.end(), data , commentLessThan);
    auto last = it - m_comments.begin();
    beginInsertRows(QModelIndex(), last, last);
    m_comments.insert(it, data);
    endInsertRows();
    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
}

void CommentsModel::clear()
{
    beginRemoveRows(QModelIndex(), 0, m_comments.count());
    m_comments.clear();
    endRemoveRows();
}

void CommentsModel::getComments(int offset, int count)
{
    if (m_session.data() && m_postId) {
        auto reply = m_session.data()->getComments(offset, count);
        connect(reply, SIGNAL(resultReady(QVariant)), SIGNAL(requestFinished()));
    }
}

