#include "chart_plugin/qcp_modify.h"
#include "utility/static_container.h"

QCPModify::QCPModify(QCPAxis* keyAxis, QCPAxis* valueAxis)
    : QCPBars (keyAxis, valueAxis),
      mTextAlignment(Qt::AlignCenter),
      mSpacing(5),
      mFont(QFont(QLatin1String("sans serif"), 9)) {
}

void QCPModify::setTextAlignment(Qt::Alignment alignment) {
    mTextAlignment = alignment;
}

void QCPModify::setSpacing(double spacing) {
    mSpacing = spacing;
}

void QCPModify::setFont(const QFont& font) {
    mFont = font;
}

void QCPModify::setLowLimit(double limit) {
    if(!limit) {
        return;
    }
    m_lowLimit = limit;
}

/**
 * @brief QCPModify::getTicks
 * @return
 */
const QVector<double> QCPModify::getTicks() {
    return mKeyAxis->tickVector();
}

///
/// \brief QCPModify::getLabels
/// \param idx
/// \return
///
const QString QCPModify::getLabels(int idx) {
    if (!mKeyAxis || !mValueAxis) {
        qDebug() << Q_FUNC_INFO << "invalid key or value axis";
        return "";
    }
    QVector<QString> vs_labels = mKeyAxis->tickVectorLabels();
    if(idx < 0) {
        return vs_labels.toList().join(",");
    }
    int i_labelCnt = vs_labels.count();
    if(i_labelCnt < 1 || idx > i_labelCnt) {
        qDebug() << Q_FUNC_INFO << "invalid label index";
        return "";
    }
    return vs_labels.at(idx);
}

QVector<double> QCPModify::getTickVector() const {
    if (!mKeyAxis || !mValueAxis) {
        qDebug() << Q_FUNC_INFO << "invalid key or value axis";
        return {};
    }
    return mKeyAxis->tickVector();
}

QVector<QString> QCPModify::getTickVectorLabels() const {
    if (!mKeyAxis || !mValueAxis) {
        qDebug() << Q_FUNC_INFO << "invalid key or value axis";
        return {};
    }
    return mKeyAxis->tickVectorLabels();
}

/**
 * @brief QCPModify::draw
 * @param painter
 */
void QCPModify::draw(QCPPainter* painter) {
    if (!mKeyAxis || !mValueAxis) {
        qDebug() << Q_FUNC_INFO << "invalid key or value axis";
        return;
    }
    if (mDataContainer->isEmpty()) {
        return;
    }
    QCPBarsDataContainer::const_iterator visibleBegin, visibleEnd;
    getVisibleDataBounds(visibleBegin, visibleEnd);
    // loop over and draw segments of unselected/selected data:
    QList<QCPDataRange> selectedSegments, unselectedSegments, allSegments;
    getDataSegments(selectedSegments, unselectedSegments);
    allSegments << unselectedSegments << selectedSegments;
    for (int i = 0; i < allSegments.size(); ++i) {
        bool isSelectedSegment = i >= unselectedSegments.size();
        QCPBarsDataContainer::const_iterator begin = visibleBegin;
        QCPBarsDataContainer::const_iterator end = visibleEnd;
        mDataContainer->limitIteratorsToDataRange(begin, end, allSegments.at(i));
        if (begin == end) {
            continue;
        }
        for (QCPBarsDataContainer::const_iterator it = begin; it != end; it++) {
            // check data validity if flag set:
#ifdef QCUSTOMPLOT_CHECK_DATA
            if (QCP::isInvalidData(it->key, it->value)) {
                qDebug() << Q_FUNC_INFO << "Data point at" << it->key << "of drawn range invalid." << "Plottable name:" << name();
            }
#endif
            // draw bar:
            if (isSelectedSegment && mSelectionDecorator) {
                mSelectionDecorator->applyBrush(painter);
                mSelectionDecorator->applyPen(painter);
            } else {
                painter->setBrush(mBrush);
                painter->setPen(mPen);
            }
            applyDefaultAntialiasingHint(painter);
            QRectF barRect = getBarRect(it->key, it->value);
            painter->drawPolygon(barRect);
            painter->setFont(mFont);    // set font
            QString text = "";          // value text initialize
            int i_valIdx = StaticContainer::STC_SELMASS.indexOf(it->key);
            if(i_valIdx > -1) {
                text = getLabels(it->key - 1) + ":" + QString::number(it->value, 'e', 1);
            }
            QRectF textRect = painter->fontMetrics().boundingRect(0, 0, 0, 0, Qt::TextDontClip | mTextAlignment, text);  // calculate text width
            if (mKeyAxis.data()->orientation() == Qt::Horizontal) {    // when key axis is horizontal
                if (mKeyAxis.data()->axisType() == QCPAxis::atTop) {   // top axis, move text to the bottom of bar
                    textRect.moveTopLeft(barRect.bottomLeft() + QPointF(0, mSpacing));
                } else {                                               // bottom axis, move text to the top of bar
                    textRect.moveBottomLeft(barRect.topLeft() - QPointF(0, mSpacing));
                }
                textRect.setWidth(barRect.width());
                painter->drawText(textRect, Qt::TextDontClip | mTextAlignment, text);
            } else {                                                  // when key axis is vertical
                if (mKeyAxis.data()->axisType() == QCPAxis::atLeft) { // left axis, move text to the right of bar
                    textRect.moveTopLeft(barRect.topRight() + QPointF(mSpacing, 0));
                } else {                                              // right axis, move text to the left of bar
                    textRect.moveTopRight(barRect.topLeft() - QPointF(mSpacing, 0));
                }
                textRect.setHeight(barRect.height());
                painter->drawText(textRect, Qt::TextDontClip | mTextAlignment, text);
            }
        }
    }
    // draw other selection decoration that isn't just line/scatter pens and brushes:
    if (mSelectionDecorator) {
        mSelectionDecorator->drawDecoration(painter, selection());
    }
}
