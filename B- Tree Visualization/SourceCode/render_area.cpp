#include "render_area.h"
#include<vector>
using namespace std;

RenderArea::RenderArea(QWidget *parent, BTree* btree, int sliderVal)
    : QWidget{parent}, btree(btree), sliderVal(sliderVal)
{
    fSize = 13;
    lengthSize = 35*1.2;
    widthSize = 35;
    rowSpace = 70;

    nodeColor = Qt::white;
    nodeSearchingColor = Qt::red;
    textColor = Qt::black;
    foundTextColor = Qt::green;

    brush = new QBrush();
    pen = new QPen();
    pen->setWidth(3);
}

RenderArea::~RenderArea()
{
    delete pen;
    delete brush;
    //delete btree;
}

void RenderArea::paintEvent (QPaintEvent*)
{
    if (btree->getRoot() == NULL) return;
    QPainter* painter = new QPainter(this);
    painter->setRenderHint(QPainter::Antialiasing);
    brush->setColor(this->nodeColor);
    brush->setStyle(Qt::SolidPattern);
    pen->setColor(this->textColor);
    painter->setBrush(*brush);
    painter->setPen(*pen);
    DrawBTree(painter, btree->getRoot(), X, Y);
    painter->end();
}

void RenderArea::SetMaxDeg(int maxDeg)
{
    if (btree->getRoot() != NULL) btree->clear();
    btree->setOrder(maxDeg);
}

void RenderArea::constructNode(QPainter* painter, QString s, double x, double y)
{

    QRectF rectNode = QRectF(QPointF(x,y),QSizeF(lengthSize, widthSize));
    painter->drawRect(rectNode);
    while (s.size() < 4) s = "0" + s;
    QStaticText text;
    text.setTextWidth(widthSize - 2.0);
    text.setText(s);
    painter->drawStaticText(QPoint(x + 4.0, y + 7.0), text);
}

double RenderArea::DrawBTree(QPainter* painter, BTreeNode* root, double x, double y)
{
    if (!root) return 0;
    if(root ->isLeaf() == false)
    {
        vector<QPoint> ChildCoor;
        double oldStartX = x,nextStartX;
        //Draw nodes
        for(int i = 0;i<=root->getSize();i++)
        {
            nextStartX = DrawBTree(painter,root->getChild(i),oldStartX, y+ rowSpace);
            double endline = (nextStartX + oldStartX)/2;
            ChildCoor.push_back(QPoint(endline,y+rowSpace));
            oldStartX = nextStartX + lengthSize;
        }

        double curStartX = (oldStartX + x)/2 - 1.0*(root->getSize()+1)*lengthSize + lengthSize/2.0;
        for(int i = 0;i<root->getSize();i++)
        {
            constructNode(painter,toString(root->getKey(i)),curStartX + i*lengthSize ,y);
        }

        //MatchLine
        double startY = y + 2*fSize, startLine;
        for (int i=0;i<=root->getSize();++i)
        {
            startLine = curStartX + i * lengthSize;
            QLineF line = QLineF(QPoint(startLine,startY), ChildCoor[i]);
            painter->drawLine(line);
        }
        return nextStartX;
    }
    else
    {
        for(int i = 0;i<root->getSize();i++)
        {
            constructNode(painter,toString(root->getKey(i)),x + i*lengthSize ,y);
        }
        return x+root->getSize()*lengthSize;
    }
}

QString RenderArea::toString(int x)
{
    QString res = "";
    while (x)
    {
        res = char(x % 10 + '0') + res;
        x /= 10;
    }
    while (res.size() < 4) res = "0" + res;
    return res;
}

void RenderArea::insert(int key)
{
    btree->insert(key);
}

bool RenderArea::remove(int key)
{
    bool rm = btree->remove(key);
    return rm;
}

bool RenderArea::search(int key)
{
    if (btree->search(key) == NULL)
        return false;
    return true;
}

void RenderArea::clear()
{
    btree->clear();
}

QString RenderArea::print()
{
    if (!btree->getRoot()) return "";
    return btree->traverse();
}

