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

#ifndef QNEBLOCK_H
#define QNEBLOCK_H

#include <QGraphicsPathItem>

class QNEPort;

class QNEBlock : public QGraphicsPathItem
{
public:
	enum { Type = QGraphicsItem::UserType + 3 };

    QNEBlock(QGraphicsItem *parent = 0);

    void setName(const QString &name);
    const QString& name() const { return m_name; }

    void setId(const QString &id);
    const QString& id() const { return m_id; }

    void refreshGeometry();

    QNEPort* addInputPort(const QString &name);
    QNEPort* addOutputPort(const QString &name);
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
	QVector<QNEPort*> ports();

	int type() const { return Type; }

    q::util::Signal<void(const QPointF&)> positionChangedSignal;

protected:
	QVariant itemChange(GraphicsItemChange change, const QVariant &value);

private:
    QNEPort* addPort(const QString &name, bool isOutput);

    QString m_name;
    QString m_id;
    int m_horzMargin;
    int m_vertMargin;
    int m_width;
    int m_height;
    QGraphicsTextItem* m_label = nullptr;
};

#endif // QNEBLOCK_H
