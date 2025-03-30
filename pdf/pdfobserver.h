/*
 * Copyright (C) 2017 Caliste Damien.
 * Contact: Damien Caliste <dcaliste@free.fr>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; version 2 only.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef PDFOBSERVER_H
#define PDFOBSERVER_H

#include <QtQuick/QQuickItem>

class PDFObserver : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(bool active READ active WRITE setActive NOTIFY activeChanged)

public:
    PDFObserver(QQuickItem *parent = 0);
    ~PDFObserver();

    bool active() const;
    void setActive(bool value);

signals:
    void activeChanged();

protected:
    virtual void geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry);
    virtual QSGNode* updatePaintNode(QSGNode *node, UpdatePaintNodeData*);

private:
    bool m_active;
};

Q_DECLARE_METATYPE(PDFObserver*)

#endif // PDFOBSERVER_H
