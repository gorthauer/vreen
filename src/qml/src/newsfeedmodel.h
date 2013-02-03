/****************************************************************************
**
** Vreen - vk.com API Qt bindings
**
** Copyright © 2012 Aleksey Sidorov <gorthauer87@ya.ru>
**
*****************************************************************************
**
** $VREEN_BEGIN_LICENSE$
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
** $VREEN_END_LICENSE$
**
****************************************************************************/
#ifndef NEWSFEEDMODEL_H
#define NEWSFEEDMODEL_H

#include <QAbstractListModel>
#include <newsfeed.h>
#include <QPointer>

namespace Vreen {
class Contact;
} //namespace Vreen

class NewsFeedModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(QObject* client READ client WRITE setClient NOTIFY clientChanged)
    Q_PROPERTY(int count READ count NOTIFY requestFinished)
public:

    enum Roles {
        TypeRole = Qt::UserRole,
        PostIdRole,
        SourceRole,
        DateRole,
        BodyRole,
        AttachmentsRole,
        LikesRole,
        RepostsRole,
        CommentsRole,
        OwnerRole,
        LikesCount,
        CommentsCount
    };

    explicit NewsFeedModel(QObject *parent = 0);
    QObject* client() const;
    void setClient(QObject* arg);
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    virtual int rowCount(const QModelIndex &) const;
    int count() const;
    int findNews(int id);
public slots:
    Vreen::Reply *getNews(int filters = Vreen::NewsFeed::FilterPost | Vreen::NewsFeed::FilterPhoto,
                       quint8 count = 10, int offset = 0);
    void addLike(int postId, bool retweet = false, const QString &message = QString());
    void deleteLike(int postId);
    void clear();
    void truncate(int count);
signals:
    void clientChanged(QObject* client);
    void requestFinished();
protected:
    void insertNews(int index, const Vreen::NewsItem &data);
    void replaceNews(int index, const Vreen::NewsItem &data);
    inline Vreen::Contact *findContact(int id) const;
private slots:
    void onNewsReceived(const Vreen::NewsItemList &data);
    void onAddLike(const QVariant &response);
    void onDeleteLike(const QVariant &response);
private:
    QPointer<Vreen::Client> m_client;
    QPointer<Vreen::NewsFeed> m_newsFeed;
    Vreen::NewsItemList m_newsList;
    Qt::SortOrder m_sortOrder;
};

#endif // NEWSFEEDMODEL_H

