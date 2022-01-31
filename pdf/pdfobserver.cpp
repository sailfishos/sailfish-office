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

#include "pdfobserver.h"

PDFObserver::PDFObserver(QQuickItem *parent)
  : QQuickItem(parent), m_active(false)
{
}

PDFObserver::~PDFObserver()
{
}

bool PDFObserver::active() const
{
    return m_active;
}

void PDFObserver::setActive(bool value)
{
    if (value == m_active)
        return;

    m_active = value;
    emit activeChanged();

    if (m_active)
        update();
}

void PDFObserver::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    if (m_active)
        update();
}

QSGNode* PDFObserver::updatePaintNode(QSGNode *node, QQuickItem::UpdatePaintNodeData *)
{
    if (!m_active)
        return nullptr;

    QSGNode *root = static_cast<QSGNode*>(node);
    if (!root) {
        root = new QSGNode;
    }

    for (int i = 0; i < d->pageCount; ++i) {
        PDFPage &page = d->pages[i];

        QSGTransformNode *t = static_cast<QSGTransformNode*>(root->childAtIndex(i));
        if (!t) {
            t = new QSGTransformNode;
            t->setFlag(QSGNode::OwnedByParent);
            root->appendChildNode(t);
        }

        QMatrix4x4 m;
        m.translate(0, page.rect.y());
        t->setMatrix(m);

        if (showPage) {
            QRectF inter = page.rect.intersected(visibleArea);
            qreal area = inter.width() * inter.height();
            // Select the current page as the page with the maximum
            // visible area.
            if (area > maxVisibleArea) {
                maxVisibleArea = area;
                currentPage = i + 1;
            }
        }

        if (showPage) {
            // Node hierachy:
            // t
            // |-bg
            // |  |-tn
            // |  | |- patch1...
            // |  |-n
            // |  | |- link1
            // |  | |- link2...
            QSGSimpleRectNode *bg = static_cast<QSGSimpleRectNode*>(t->firstChild());
            if (!bg) {
                bg = new QSGSimpleRectNode;
                bg->setFlag(QSGNode::OwnedByParent);
                bg->setColor(d->pagePlaceholderColor);
                t->appendChildNode(bg);
            }
            bg->setRect(0., 0., page.rect.width(), page.rect.height());

            if (page.texture) {
                QSGSimpleTextureNode *tn = static_cast<QSGSimpleTextureNode *>(bg->firstChild());
                if (!tn) {
                    tn = new QSGSimpleTextureNode;
                    tn->setFlag(QSGNode::OwnedByParent);
                    bg->appendChildNode(tn);
                }
                putTexture(tn, width(), page.renderWidth, page.textureArea, page.texture);

                if (!page.patches.empty()) {
                    QSGSimpleTextureNode *ptn = static_cast<QSGSimpleTextureNode*>(tn->firstChild());
                    for (QList<Patch>::iterator it = page.patches.begin();
                         it != page.patches.end(); it++) {
                        if (!ptn) {
                            ptn = new QSGSimpleTextureNode;
                            ptn->setFlag(QSGNode::OwnedByParent);
                            tn->appendChildNode(ptn);
                        }
                        
                        putTexture(ptn, width(), page.renderWidth, it->first, it->second);

                        ptn = static_cast<QSGSimpleTextureNode*>(ptn->nextSibling());
                    }
                } else {
                    // Delete all previously registered patches.
                    for (QSGNode *child = tn->firstChild(); child; child = tn->firstChild())
                        delete child;
                }
            } else {
                delete bg->firstChild(); // delete the texture root here.
            }
        } else {
            delete t->firstChild();
        }
    }

    return root;
}
