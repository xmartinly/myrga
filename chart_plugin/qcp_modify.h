#ifndef QCPMODIFY_H
#define QCPMODIFY_H


class QCPModify: public QCPBars {

  public:
    explicit QCPModify(QCPAxis* keyAxis, QCPAxis* valueAxis);

    Qt::Alignment textAligment() const {
        return mTextAlignment;
    }
    double spacing() const {
        return mSpacing;
    }

    QFont font() const {
        return mFont;
    }

    void setTextAlignment(Qt::Alignment alignment);
    void setSpacing(double spacing);
    void setFont(const QFont& font);
    void setLowLimit(double low_limit = 0);

    const QVector<double> getTicks();

    const QString getLabels(int idx = -1);

    QVector<double> getTickVector() const;
    QVector<QString> getTickVectorLabels() const;


  protected:
    Qt::Alignment mTextAlignment;   // text alignment
    double mSpacing;                // space from the text to bar, pixel
    QFont mFont;                    // text font

    double m_lowLimit = 5e-12;

    virtual void draw(QCPPainter* painter) Q_DECL_OVERRIDE;
};


#endif // QCPMODIFY_H
