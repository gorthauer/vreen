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
#ifndef AUDIOMODEL_H
#define AUDIOMODEL_H
#include <audio.h>
#include <QPointer>
#include <client.h>

namespace Vreen {
class Contact;
}

class AudioModel : public Vreen::AudioModel
{
    Q_OBJECT

    Q_PROPERTY(Vreen::Client* client READ client WRITE setClient NOTIFY clientChanged)
public:
    explicit AudioModel(QObject *parent = 0);
    Vreen::Client* client() const;
    void setClient(Vreen::Client *client);
public slots:
    void getAudio(int id = 0, int count = 100, int offset = 0);
    void getAudio(Vreen::Contact *contact, int count = 100, int offset = 0);
    void searchAudio(const QString& query, int count = 50, int offset = 0, bool autoComplete = true, Vreen::AudioProvider::SortOrder sort = Vreen::AudioProvider::SortByPopularity, bool withLyrics = false);
signals:
    void clientChanged(Vreen::Client *client);
    void requestFinished();
private slots:
    void onResultReady();
private:
    QPointer<Vreen::Client> m_client;
    QPointer<Vreen::AudioProvider> m_provider;
};

#endif // AUDIOMODEL_H

