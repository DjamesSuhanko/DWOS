#ifndef GRAPH_BASE_H
#define GRAPH_BASE_H

#include <QtGui>
#include <QWidget>
#include <QDebug>
#include <QLabel>
#include <QApplication>

class GraphRealTime : public QWidget {

    Q_OBJECT

private:
	QColor colorAxis;
	QColor colorAxisGrid;
	QColor colorBackground;
	QColor colorLimitWarning;
	QColor colorLimitCritical;
	QColor colorLine;
	int dLenX;
	int dLenY;
	int dSize;
	qreal dX;
	qreal dY;
	int fontSize;
	qreal limitCritical1;
	qreal limitCritical2;
	qreal limitWarning1;
	qreal limitWarning2;
	int marginBottom;
	int marginLeft;
	int marginTop;
	int marginRight;
	qreal maxX;
	qreal maxY;
	qreal minX;
	qreal minY;
	QList<QPointF> points;
	qreal scaleX;
	qreal scaleY;

	QString toStringXValue(qreal value);
	QString toStringYValue(qreal value);


public:
	explicit GraphRealTime(QWidget * parent=0);
	~GraphRealTime();

	//! Label que recebe o nome do sensor e plota com transparência sobre o gráfico
    QLabel *itemName;

	bool addPoint(QPointF data);
	bool setColors(QColor background, QColor axis, QColor grid, QColor warning, QColor critical, QColor line);
	bool setFont(int size);
	bool setLen(int x, int y);
	bool setLimitCritical(qreal limit1, qreal limit2);
	bool setLimitWarning(qreal limit1, qreal limit2);
	bool setMarginConfig(int mTop=0, int mBottom=0, int mLeft=0, int mRight=0, int mSize=10);
	bool setSize(int w, int h);
	bool setValuesX(qreal min, qreal max);
	bool setValuesY(qreal min, qreal max);
    bool setLabelText(QString text);
	void clear();

public slots:
    void addNewValue(int x, int y);

protected:
	virtual void paintEvent(QPaintEvent *e);
	virtual void resizeEvent(QResizeEvent *e);

};

#endif // GRAPH_BASE_H
