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
#ifndef CONTACTSMODEL_H
#define CONTACTSMODEL_H

#include <QAbstractListModel>
#include <roster.h>
#include <utils.h>
#include <QPointer>

class BuddyModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(Vreen::Roster *roster READ roster WRITE setRoster NOTIFY rosterChanged)
    Q_PROPERTY(QString filterByName READ filterByName WRITE setFilterByName NOTIFY filterByNameChanged)
public:
    enum Roles {
        ContactRole = Qt::UserRole + 1
    };

    struct CompareType
    {
        QString name;
        int status;
        inline friend bool operator <(const BuddyModel::CompareType &a, const BuddyModel::CompareType &b)
        {
            int less = a.name.compare(b.name);
            if (less)
                return less > 0;
            return a.status < b.status;
        }
        inline static CompareType comparator(Vreen::Buddy * const& buddy)
        {
            BuddyModel::CompareType type = {
                buddy->name(),
                buddy->status()
            };
            return type;
        }
    };

    explicit BuddyModel(QObject *parent = 0);
    
    void setRoster(Vreen::Roster *roster);
    Vreen::Roster *roster() const;
    int count() const;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    virtual int rowCount(const QModelIndex &parent) const;
    void setFilterByName(const QString &filter);
    QString filterByName();
    void clear();
public slots:
    int findContact(int id) const;
signals:
    void rosterChanged(Vreen::Roster*);
    void filterByNameChanged(const QString &filter);
private slots:
    void addBuddy(Vreen::Buddy *);
    void removeFriend(int id);
    void onSyncFinished();
protected:
    bool checkContact(Vreen::Buddy *);
    void setBuddies(const Vreen::BuddyList &list);
private:
    QPointer<Vreen::Roster> m_roster;
    Vreen::BuddyList m_buddyList;
    QString m_filterByName;
    bool m_friendsOnly;

    Vreen::Comparator<Vreen::Buddy*, CompareType> m_buddyComparator;
};

#endif // CONTACTSMODEL_H

