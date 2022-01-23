/*
 * Copyright (C) 2016 Caliste Damien.
 * Copyright (C) 2022 Yura Beznos <yura.beznos@you-ra.info>
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

#ifndef DJVUSELECTION_H
#define DJVUSELECTION_H

#include <QtCore/QAbstractListModel>

#include "DJVUcanvas.h"

class DJVUSelection : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(DJVUCanvas* canvas READ canvas WRITE setCanvas NOTIFY canvasChanged)
    Q_PROPERTY(QPointF handle1 READ handle1 WRITE setHandle1 NOTIFY handle1Changed)
    Q_PROPERTY(QPointF handle2 READ handle2 WRITE setHandle2 NOTIFY handle2Changed)
    Q_PROPERTY(float handle1Height READ handle1Height NOTIFY handle1Changed)
    Q_PROPERTY(float handle2Height READ handle2Height NOTIFY handle2Changed)
    Q_PROPERTY(QString text READ text NOTIFY textChanged)
    Q_PROPERTY(float wiggle READ wiggle WRITE setWiggle NOTIFY wiggleChanged)

public:
    enum DJVUSelectionRoles {
        Rect = Qt::UserRole + 1,
        Text
    };
    explicit DJVUSelection(QObject *parent = 0);
    virtual ~DJVUSelection();

    virtual QVariant data(const QModelIndex &index, int role) const;
    virtual int rowCount(const QModelIndex &parent) const;
    virtual QHash<int, QByteArray> roleNames() const;

    QPair<int, QRectF> rectAt(int index) const;

    int count() const;
    DJVUCanvas* canvas() const;
    void setCanvas(DJVUCanvas *newCanvas);
    float wiggle() const;
    void setWiggle(float newValue);

    /**
     * Change current selection to match the word that is at point in canvas coordinates.
     * If there is no word at point, the selection is invalidated (ie. count is set to
     * zero).
     */
    Q_INVOKABLE bool selectAt(const QPointF &point);
    Q_INVOKABLE void unselect();

    /**
     * Check if point is inside selection.
     */
    bool selectionAtPoint(const QPointF &point) const;

    /**
     * Return a point for the start handle of the selection in canvas coordinates.
     * This handle can be dragged later and become the stop handle.
     */
    QPointF handle1() const;
    float handle1Height() const;
    /**
     * Change the start/stop of the selection to the start/end of the word at point.
     * point is given in canvas coordinates. If there is no word at point,
     * the selection is left unchanged.
     */
    void setHandle1(const QPointF &point);

    /**
     * Return a point for the stop handle of the selection in canvas coordinates.
     * This handle can later be dragged to become the start handle.
     */
    QPointF handle2() const;
    float handle2Height() const;
    /**
     * Change the stop of the selection to the end of the word at point.
     * point is given in canvas coordinates. If there is no word at point,
     * the selection is left unchanged.
     */
    void setHandle2(const QPointF &point);

    QString text() const;

Q_SIGNALS:
    void countChanged();
    void canvasChanged();
    void handle1Changed();
    void handle2Changed();
    void textChanged();
    void wiggleChanged();

private:
    class Private;
    Private * const d;

    void setStart(const QPointF &point);
    void setStop(const QPointF &point);

    void onLayoutChanged();
};

#endif // DJVUSELECTION_H
