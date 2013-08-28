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
#include "buddymodel.h"
#include <contact.h>
#include <utils.h>
#include <QApplication>

#include <QDebug>

BuddyModel::BuddyModel(QObject *parent) :
    QAbstractListModel(parent),
    m_friendsOnly(true),
    m_buddyComparator(BuddyModel::CompareType::comparator, Qt::AscendingOrder)
{
    auto roles = roleNames();
    roles[ContactRole] = "contact";
    setRoleNames(roles);
}

void BuddyModel::setRoster(Vreen::Roster *roster)
{
    if (!m_roster.isNull())
        m_roster.data()->disconnect(this);
    m_roster = roster;

    setBuddies(roster->buddies());
    connect(roster, SIGNAL(buddyAdded(Vreen::Buddy*)), SLOT(addBuddy(Vreen::Buddy*)));
    connect(roster, SIGNAL(buddyRemoved(int)), SLOT(removeFriend(int)));
    connect(roster, SIGNAL(syncFinished(bool)), SLOT(onSyncFinished()));
    emit rosterChanged(roster);
}

void BuddyModel::setBuddies(const Vreen::BuddyList &list)
{
    clear();
    beginInsertRows(QModelIndex(), 0, list.count());
    m_buddyList = list;
    endInsertRows();
}

Vreen::Roster *BuddyModel::roster() const
{
    return m_roster.data();
}

int BuddyModel::count() const
{
    return m_buddyList.count();
}

QVariant BuddyModel::data(const QModelIndex &index, int role) const
{
    int row = index.row();
    switch (role) {
    case ContactRole: {
        auto contact = m_buddyList.at(row);
        return qVariantFromValue(contact);
        break;
    }
    default:
        break;
    }
    return QVariant();
}

int BuddyModel::rowCount(const QModelIndex &parent) const
{
    Q_ASSERT(parent == QModelIndex());
    Q_UNUSED(parent);
    return count();
}

void BuddyModel::setFilterByName(const QString &filter)
{
    m_filterByName = filter;
    emit filterByNameChanged(filter);

    Vreen::BuddyList list;
    for (auto & buddy : m_roster->buddies()) {
        if (checkContact(buddy)) {
            auto it = qLowerBound(std::begin(list), std::end(list), buddy, m_buddyComparator);
            list.insert(it, buddy);
        }
    }
    setBuddies(list);
}

QString BuddyModel::filterByName()
{
    return m_filterByName;
}

void BuddyModel::clear()
{
    beginResetModel();
    m_buddyList.clear();
    endResetModel();
}
int BuddyModel::findContact(int id) const
{
    for (int i = 0; i != m_buddyList.count(); i++)
        if (m_buddyList.at(i)->id() == id)
            return i;
    return -1;
}

void BuddyModel::addBuddy(Vreen::Buddy *contact)
{
    if (!checkContact(contact))
        return;
    auto it = qLowerBound(std::begin(m_buddyList), std::end(m_buddyList), contact, m_buddyComparator);
    int index = std::distance(std::begin(m_buddyList), it);

    beginInsertRows(QModelIndex(), index, index);
    m_buddyList.insert(index, contact);
    endInsertRows();
}

void BuddyModel::removeFriend(int id)
{
    int index = findContact(id);
    if (index == -1)
        return;
    beginRemoveRows(QModelIndex(), index, index);
    m_buddyList.removeAt(index);
    endRemoveRows();
}

void BuddyModel::onSyncFinished()
{
    setBuddies(m_roster->buddies());
}

bool BuddyModel::checkContact(Vreen::Buddy *contact)
{
    if (m_friendsOnly && !contact->isFriend())
        return false;
    if (!m_filterByName.isEmpty())
        return contact->name().contains(m_filterByName, Qt::CaseInsensitive);
    return true;
}
