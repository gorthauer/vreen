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
#include "newsfeedmodel.h"
#include <QApplication>
#include <roster.h>
#include <client.h>
#include <utils.h>
#include <groupmanager.h>
#include <QDateTime>
#include <QNetworkReply>

NewsFeedModel::NewsFeedModel(QObject *parent) :
    QAbstractListModel(parent),
    m_sortOrder(Qt::DescendingOrder)
{
    auto roles = roleNames();
    roles[TypeRole] = "type";
    roles[PostIdRole] = "postId";
    roles[SourceRole] = "source";
    roles[DateRole] = "date";
    roles[BodyRole] = "body";
    roles[AttachmentsRole] = "attachments";
    roles[LikesRole] = "likes";
    roles[RepostsRole] = "reposts";
    roles[CommentsRole] = "comments";
    roles[OwnerRole] = "owner";
    roles[LikesCount] = "likesCount";
    roles[CommentsCount] = "commentsCount";
    setRoleNames(roles);
}

QObject *NewsFeedModel::client() const
{
    return m_client.data();
}

void NewsFeedModel::setClient(QObject *obj)
{
    auto client = static_cast<Vreen::Client*>(obj);
    m_client = client;
    if (m_newsFeed.data())
        m_newsFeed.data()->deleteLater();
    if (!client)
        return;

    auto newsFeed = new Vreen::NewsFeed(client);
    connect(newsFeed, SIGNAL(newsReceived(Vreen::NewsItemList)), SLOT(onNewsReceived(Vreen::NewsItemList)));

    m_newsFeed = newsFeed;
}

QVariant NewsFeedModel::data(const QModelIndex &index, int role) const
{
    int row = index.row();

    auto news = m_newsList.at(row);
    switch (role) {
    case TypeRole:
        return news.type();
    case PostIdRole:
        return news.postId();
        break;
    case SourceRole: {
        int source = news.sourceId();
        return qVariantFromValue(findContact(source));
    }
    case DateRole:
        return news.date();
    case BodyRole:
        return news.body();
    case AttachmentsRole:
        return Vreen::Attachment::toVariantMap(news.attachments());
    case LikesRole:
        return news.likes();
    case RepostsRole:
        return news.reposts();
    case CommentsRole:
        return news.property("comments");
    case OwnerRole: {
        int ownerId = news.property("copy_owner_id").toInt();
        if (ownerId) {
            auto contact = findContact(ownerId);
            return QVariant::fromValue(contact);
        }
        return QVariant();
    }
    case LikesCount: {
        return news.likes().value("count").toInt();
    }
    case CommentsCount:
        return news.property("comments").toMap().value("count").toInt();
    default:
        break;
    }
    return QVariant::Invalid;
}

int NewsFeedModel::rowCount(const QModelIndex &) const
{
    return count();
}

int NewsFeedModel::count() const
{
    return m_newsList.count();
}

Vreen::Reply *NewsFeedModel::getNews(int filters, quint8 count, int offset)
{
    if (m_newsFeed.isNull())
        return 0;

    auto reply = m_newsFeed.data()->getNews(Vreen::NewsFeed::Filters(filters), count, offset);
    connect(reply, SIGNAL(resultReady(QVariant)), SIGNAL(requestFinished()));
    return reply;
}

void NewsFeedModel::addLike(int postId, bool retweet, const QString &message)
{
    int index = findNews(postId);
    if (index != -1) {
        auto news = m_newsList.at(index);
        auto reply = m_client.data()->addLike(news.sourceId(),
                                              postId,
                                              retweet,
                                              message);
        connect(reply, SIGNAL(resultReady(QVariant)), SLOT(onAddLike(QVariant)));
    }
}

void NewsFeedModel::deleteLike(int postId)
{
    int index = findNews(postId);
    if (index != -1) {
        auto news = m_newsList.at(index);
        auto reply = m_client.data()->deleteLike(news.sourceId(),
                                                 postId);
        connect(reply, SIGNAL(resultReady(QVariant)), SLOT(onDeleteLike(QVariant)));
    }
}

int NewsFeedModel::findNews(int id)
{
    for (int i = 0 ; i != count(); i++) {
        if (id == m_newsList.at(i).postId())
            return i;
    }
    return -1;
}

void NewsFeedModel::clear()
{
    beginRemoveRows(QModelIndex(), 0, m_newsList.count());
    m_newsList.clear();
    endRemoveRows();
}

void NewsFeedModel::truncate(int count)
{
    if (count >= m_newsList.count())
        count = m_newsList.count() - 1;

    if (count > 0) {
        beginRemoveRows(QModelIndex(), count, m_newsList.count() - 1);
        m_newsList.erase(m_newsList.begin() + count - 1, m_newsList.end());
        endRemoveRows();
    }
}

static bool newsItemMoreThan(const Vreen::NewsItem &a, const Vreen::NewsItem &b)
{
    return a.date() > b.date();
}

void NewsFeedModel::onNewsReceived(const Vreen::NewsItemList &items)
{
    foreach (auto item, items) {
        if (findNews(item.postId()) == -1) {
            auto index = Vreen::lowerBound(m_newsList, item, newsItemMoreThan);
            insertNews(index, item);
        }
    }
}

void NewsFeedModel::onAddLike(const QVariant &response)
{
    auto reply = Vreen::sender_cast<Vreen::Reply*>(sender());
    auto url = reply->networkReply()->url();

    int postId = url.queryItemValue("post_id").toInt();
    int retweet = url.queryItemValue("repost").toInt();
    auto map = response.toMap();
    int likes = map.value("likes").toInt();
    int reposts = map.value("reposts").toInt();

    int index = findNews(postId);
    if (index != -1) {
        auto news = m_newsList.at(index);

        map = news.likes();
        map.insert("count", likes);
        map.insert("user_likes", true);
        news.setLikes(map);

        map = news.reposts();
        map.insert("count", reposts);
        map.insert("user_reposted", retweet);
        news.setReposts(map);

        replaceNews(index, news);
    }
}

void NewsFeedModel::onDeleteLike(const QVariant &response)
{
    auto reply = Vreen::sender_cast<Vreen::Reply*>(sender());
    auto url = reply->networkReply()->url();

    int postId = url.queryItemValue("post_id").toInt();
    int likes = response.toMap().value("likes").toInt();
    int index = findNews(postId);
    if (index != -1) {
        auto news = m_newsList.at(index);

        auto map = news.likes();
        map.insert("count", likes);
        map.insert("user_likes", false);
        news.setLikes(map);

        map = news.reposts();
        map.insert("user_reposted", false);
        news.setReposts(map);

        replaceNews(index, news);
    }
}


void NewsFeedModel::insertNews(int index, const Vreen::NewsItem &item)
{
    beginInsertRows(QModelIndex(), index, index);
    m_newsList.insert(index, item);
    endInsertRows();
}

void NewsFeedModel::replaceNews(int i, const Vreen::NewsItem &news)
{
    auto index = createIndex(i, 0);
    m_newsList[i] = news;
    emit dataChanged(index, index);
}

Vreen::Contact *NewsFeedModel::findContact(int id) const
{
    return m_client.data()->contact(id);
}

