/* Copyright (c) 2012, STANISLAW ADASZEWSKI
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of STANISLAW ADASZEWSKI nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL STANISLAW ADASZEWSKI BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. */

#ifndef QNEPORT_H
#define QNEPORT_H

#include <QGraphicsPathItem>
#include <QVector2D>

#include "QBase.h"

class QNEBlock;
class QNEConnection;

class QNEPort : public QGraphicsPathItem
{
public:
	enum { Type = QGraphicsItem::UserType + 1 };

    QNEPort(QGraphicsItem *parent = 0);
	~QNEPort();

    void disconnectAll();

	void setNEBlock(QNEBlock*);
    void setIsOutput(bool o);
    bool isOutput();

    int radius();

    void setPortType(uint32_t type);
    uint32_t portType() const { return m_portType; }

    void setPortRate(uint32_t rate);
    uint32_t portRate() const { return m_portRate; }

    void setName(const QString &n);
    const QString& name() const { return m_name; }

    void setId(const QString &n);
    const QString& id() const { return m_id; }

    QVector<QNEConnection*>& connections();
    int type() const { return Type; }

	QNEBlock* block() const;

    void setDir(QVector2D dir);
    QVector2D dir() const;

	bool isConnected(QNEPort*);

    boost::signals2::signal<void(QNEPort*)> sig_connected;

protected:
	QVariant itemChange(GraphicsItemChange change, const QVariant &value);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

private:
    QNEBlock* m_block = nullptr;
    QString m_name;
    QString m_id;
    bool m_isOutput = false;
    QGraphicsTextItem* m_label = nullptr;
    int m_radius = 0;
    int m_margin = 0;
	QVector<QNEConnection*> m_connections;
    uint32_t m_portType = 0;
    uint32_t m_portRate = 0;
    QVector2D m_dir;
};

#endif // QNEPORT_H
