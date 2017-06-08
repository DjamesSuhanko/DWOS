#include "graphrealtime.h"

GraphRealTime::GraphRealTime(QWidget *parent)
	: QWidget(parent)
{
	// widget config
	setAttribute(Qt::WA_OpaquePaintEvent);
	setFocusPolicy(Qt::NoFocus);
    setGeometry(0, 0, 600, 300);
	setAutoFillBackground(true);
	setHidden(true);

	// init variables
	fontSize = 8;

	// set colors
    colorAxis.setRgb(255,153,51);
    colorAxisGrid.setRgb(255,153,51,50);
    colorBackground.setRgb(0,0,0);
	colorLimitWarning.setRgb(255,185,0);
	colorLimitCritical.setRgb(255,0,0);
    colorLine.setRgb(230,230,230);

	// init values
    setMarginConfig(25, 25, 25, 25);
	setLen(1, 1);
    setValuesX(0, 120);
    setValuesY(0, 10);

	setLimitCritical(-1, -1);
	setLimitWarning(-1, -1);

    //! No construtor, inicia-se o texto do label itemName
    this->itemName = new QLabel(this);
    this->itemName->setText("                                                                                                         ");
    this->itemName->setStyleSheet("QLabel {color:rgba(255,153,51,100); background:transparent; font-size:16px}");
    this->itemName->move(50,5);
    this->itemName->show();
}


GraphRealTime::~GraphRealTime()
{
}

void GraphRealTime::clear()
{
	this->points.clear();
	//refreshing
	update();

}

void GraphRealTime::addNewValue(int x, int y)
{
    addPoint(QPointF((float) x,(float) y));
    update();
}

bool GraphRealTime::addPoint(QPointF value)
{
	QPointF oldPoint;

	// check if has other points
	if(!points.isEmpty())
		oldPoint = points.last();

	// check if x is consistent
	if(!oldPoint.isNull() && value.x() < oldPoint.x())
	{
		qWarning("Graph::addPoint(): the new point's x value is less than the last point's x value.");
		return false;
	}

	// add point
	points << value;

	// refresh graph
	update();

	return true;
}

bool GraphRealTime::setColors(QColor background, QColor axis, QColor grid,
							  QColor warning, QColor critical, QColor line)
{
	colorBackground = background;
	colorAxis = axis;
	colorAxisGrid = grid;
	colorLimitWarning = warning;
	colorLimitCritical = critical;
	colorLine = line;

	update();

	return true;
}


bool GraphRealTime::setFont(int size)
{
	if(size < 1) return false;

	fontSize = size;

	return true;
}

bool GraphRealTime::setLabelText(QString text)
{
    this->itemName->setText(text);
    update();
    return true;
}

bool GraphRealTime::setLen(int x, int y)
{
	if(x < 1 || y < 1) return false;

	dLenX = x;
	dLenY = y;

	scaleX = (qreal(width()) - marginLeft - marginRight - dSize) / dLenX;
	scaleY = (qreal(height()) - marginTop - marginBottom - dSize) / dLenY;

	update();

	return true;
}


bool GraphRealTime::setLimitCritical(qreal limit1, qreal limit2)
{
	limitCritical1 = limit1;
	limitCritical2 = limit2;

	update();

	return true;
}


bool GraphRealTime::setLimitWarning(qreal limit1, qreal limit2)
{
	limitWarning1 = limit1;
	limitWarning2 = limit2;

	update();

	return true;
}


bool GraphRealTime::setMarginConfig(int mTop, int mBottom, int mLeft, int mRight, int mSize)
{
	if(mSize < 0) return false;

	marginTop = mTop;
	marginBottom = mBottom;
	marginLeft = mLeft;
	marginRight = mRight;
	dSize = mSize;

	setLen(dLenX, dLenY);

	return true;
}


bool GraphRealTime::setSize(int w, int h)
{
	// minimum size
	if(w < 50 || h < 50) return false;

	setGeometry(0, 0, w, h);
	setMinimumSize(w, h);

	setLen(dLenX, dLenY);

	return true;
}


bool GraphRealTime::setValuesX(qreal min, qreal max)
{
	minX = min;
	maxX = max;

	dX = qreal(max - min) / dLenX;

	update();

	return true;
}


bool GraphRealTime::setValuesY(qreal min, qreal max)
{
	minY = min;
	maxY = max;

	dY = qreal(max - min) / dLenY;

	update();

	return true;
}


QString GraphRealTime::toStringXValue(qreal value)
{
    return QDateTime::fromTime_t( (uint) value ).toString("mm:ss");
}


QString GraphRealTime::toStringYValue(qreal value)
{
	return QString::number( (int) value );
}


void GraphRealTime::paintEvent(QPaintEvent *e)
{
	QPainter p(this);

	p.setFont( QFont(QApplication::font().family(), fontSize) );

	p.setPen(QPen(colorAxis));

    // fill background
	p.fillRect(e->rect(), colorBackground);

	p.translate(-1,-1);

	// Y-axis
	p.drawLine(dSize+marginLeft, marginTop, dSize+marginLeft, height()-marginBottom);
    p.drawText(0, height()-marginBottom-dSize-10, marginLeft-1, 14, Qt::AlignRight, toStringYValue(minY));

	// X-axis
	p.drawLine(marginLeft, height()-marginBottom-dSize, width()-marginRight, height()-marginBottom-dSize);
    p.drawText(marginLeft-(scaleX/2)+dSize, height()-marginBottom+2, scaleX, 10, Qt::AlignCenter, toStringXValue(minX));


	// create aux variables
	qreal posY, posX, valueX, valueY;

	// Y-axis values
	posY = marginTop;
	valueY = maxY;

	for(int i=dLenY; i > 0; i--)
	{
		p.drawLine(marginLeft, posY, marginLeft+dSize, posY);
        p.drawText(0, posY-5, marginLeft-1, 14, Qt::AlignRight, toStringYValue(valueY));

		// draw grid
		p.setPen(QPen(colorAxisGrid));
		p.drawLine(marginLeft+dSize, posY, width()-marginRight, posY);

		// normal color axis
		p.setPen(QPen(colorAxis));

		// next pos
		posY += scaleY;
		valueY -= dY;
	}

	// X-axis values
	posX = width()-marginRight;
	posY = height()-marginBottom;
	valueX = maxX;

	for(int i=dLenX; i > 0; i--)
	{
		p.drawLine(posX, posY, posX, posY-dSize);
		p.drawText(posX-(scaleX/2), posY+2, scaleX, 10, Qt::AlignCenter, toStringXValue(valueX));

		// draw grid
		p.setPen(QPen(colorAxisGrid));
		p.drawLine(posX, posY-dSize, posX, marginTop);

		// normal color axis
		p.setPen(QPen(colorAxis));

		// next pos
		posX -= scaleX;
		valueX -= dX;
	}

	// set translate to plot graph
	p.translate(marginLeft+dSize, height()-marginBottom-dSize);

	// set scale to plot graph
	p.scale( qreal(width()-marginRight-marginLeft-dSize) / (maxX-minX), -(qreal(height()-marginBottom-marginTop-dSize) / (maxY-minY)) );

	// set translate to plot graph
	p.translate(-minX, -minY);

	// draw Critical Line
	p.setPen(QPen(colorLimitCritical));
	if(limitCritical1 > minY && limitCritical1 < maxY)
		p.drawLine(minX, limitCritical1, maxX, limitCritical1);
	if(limitCritical2 > minY && limitCritical2 < maxY)
		p.drawLine(minX, limitCritical2, maxX, limitCritical2);

	// draw Warning Line
	p.setPen(QPen(colorLimitWarning));
	if(limitWarning1 > minY && limitWarning1 < maxY)
		p.drawLine(minX, limitWarning1, maxX, limitWarning1);
	if(limitWarning2 > minY && limitWarning2 < maxY)
		p.drawLine(minX, limitWarning2, maxX, limitWarning2);

	// no line to draw
	if(points.length() < 2) return;

	// change to line color
	p.setPen(QPen(colorLine));

	QListIterator<QPointF> iterator(points);

	QPointF point_old;
	QPointF point_new;

	while(iterator.hasNext())
	{
		point_old = iterator.next();

		// point to remove
		if(point_old.x() < minX)
		{
			points.removeFirst();
			continue;
		}

		break;
	}


	// check if has line to draw
	if(points.length() < 2) return;

	QVarLengthArray<QLineF> lines;

	while(iterator.hasNext())
	{
		point_new = iterator.next();

		// point out of graph
		if(point_new.x() > maxX)
			break;

		lines.append(QLineF(point_old, point_new));

		point_old = point_new;
	}

    QPen s;
    s.setWidthF(0.3);
    QColor cor;
    //cor.setGreen(255);
    cor.setNamedColor("white");
    s.setColor(cor);
    p.setPen(QPen(s));
	p.drawLines(lines.constData(), lines.size());
    update();
}


void GraphRealTime::resizeEvent(QResizeEvent *)
{
	setLen(dLenX, dLenY);
	update();
}
