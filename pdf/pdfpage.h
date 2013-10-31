/*
 * This file is part of the KDE project
 * Copyright (C) 2013 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef PDFPAGE_H
#define PDFPAGE_H

#include <qquickpainteditem.h>

class PDFPage : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(QImage content READ content WRITE setContent NOTIFY contentChanged)

public:
    explicit PDFPage(QQuickItem *parent = 0);
    ~PDFPage();

    virtual void paint(QPainter* painter);

    QImage content() const;

public Q_SLOTS:
    void setContent(const QImage& content);

Q_SIGNALS:
    void contentChanged();

private:
    class Private;
    Private * const d;
};

#endif // PDFPAGE_H
