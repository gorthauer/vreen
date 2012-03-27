#ifndef VK_ROSTER_H
#define VK_ROSTER_H

#include "vk_global.h"
#include <QVariant>
#include <QStringList>

namespace vk {

class Client;
class Contact;
class RosterPrivate;
class VK_SHARED_EXPORT Roster : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(Roster)
public:
    Roster(Client *client);
    virtual ~Roster();
    Contact *contact(int id) const;
    Contact *contact(const QVariantMap &data);
    QStringList tags() const;
    void setTags(const QStringList &list);
public slots:
    void sync(const QStringList &filter = QStringList());
signals:
    void contactAdded(Contact *contact);
    void contactRemoved(Contact *contact);
    void tagsChanged(const QStringList &);
protected:
    QScopedPointer<RosterPrivate> d_ptr;

    //friend class Contact;
    //friend class Buddy;
    //friend class Group;

    //Q_PRIVATE_SLOT(d_func(), void _q_)
};

} // namespace vk

#endif // VK_ROSTER_H
