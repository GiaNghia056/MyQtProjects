#include "mainwindow.h"
#include "./ui_mainwindow.h"
#define sz(a) (int)a.size()
const int MaximumLength = 4;
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setFixedSize(1500,800);
    ui->textbox->setMaxLength(4);
    ui->textbox->setText(QString());

    forPrintTree = new QLabel();
    forPrintTree->setWordWrap(true);
    QFont f( "Segoe UI", 16, QFont::Bold);
    forPrintTree->setFont(f);
    headLayout = new QHBoxLayout;
    headLayout->addWidget(ui->label);
    headLayout->addWidget(ui->textbox);
    headLayout->addWidget(ui->insButton);
    headLayout->addWidget(ui->revButton);
    headLayout->addWidget(ui->searchButton);
    headLayout->addSpacing(35);
    headLayout->addWidget(ui->printButton);
    headLayout->addWidget(ui->clrButton);
    headLayout->addSpacing(35);
    headLayout->addWidget(ui->label_3);
    headLayout->addWidget(ui->maxDegOpt);

    footLayout = new QHBoxLayout;
    footLayout->addWidget(ui->history);
    footLayout->addSpacing(432);
    footLayout->addSpacing(300);

    renderArea = new RenderArea(this,new BTree(3),50);
    renderArea->setVisible(true);
    renderArea->setMinimumWidth(5000);
    renderArea->setMinimumHeight(5000);
    renderArea->setUpdatesEnabled(true);

    renderArea->SetX(600 + renderArea->geometry().x());
    renderArea->SetY(renderArea->geometry().y() + 10);


    ui->maxDegOpt->addItem("3");
    ui->maxDegOpt->addItem("4");
    ui->maxDegOpt->addItem("5");
    ui->maxDegOpt->addItem("6");
    ui->maxDegOpt->addItem("7");
    ui->maxDegOpt->setCurrentIndex(0);

    ui->scrollArea->setWidget(renderArea);
    ui->scrollArea->installEventFilter(renderArea);

    ui->textbox->setMaxLength(MaximumLength);
    QRegularExpression expr("[-,+]{0,1}[0-9]+[.,,][0-9]+");
    QRegularExpressionValidator *v = new QRegularExpressionValidator(expr, 0);
    ui->textbox->setValidator(v);

    mainLayout = new QVBoxLayout;
    mainLayout->addWidget(ui->scrollArea);
    mainLayout->addWidget(forPrintTree);
    mainLayout->addLayout(headLayout,200);
    mainLayout->addLayout(footLayout);

    centralWidget = new QWidget(this);
    centralWidget->setLayout(mainLayout);
    this->setCentralWidget(centralWidget);

}

MainWindow::~MainWindow()
{
    delete ui;
    delete renderArea;
    delete forPrintTree;
    delete headLayout;
    delete footLayout;
    delete centralWidget;
    delete mainLayout;
}


void MainWindow::on_searchButton_clicked()
{
    QString num = ui->textbox->text();
    if (num.size() == 0) return;
    ui->textbox->setText("");
    forPrintTree->setText("");
    ui->history->setText(">>>Searching key " + num + "...");
    QApplication::processEvents();
    bool succ = renderArea->search(convertToNum(num));
    if (succ)
    {
        ui->history->setText(">>>Key " + num + " was found");
    }
    else
    {
        ui->history->setText(">>>Key " + num + " does not exist in the tree");
        QMessageBox::critical(this,"Search","Key " + num + " does not exist in the tree");
    }
}


void MainWindow::on_insButton_clicked()
{
    QString num = ui->textbox->text();
    if (num.size() == 0) return;
    ui->textbox->setText("");
    forPrintTree->setText("");
    ui->history->setText(">>>Inserting key " + num + "...");

    QApplication::processEvents();
    renderArea->insert(convertToNum(num));
    renderArea->repaint();
    ui->history->setText(">>>Key " + num + " was inserted");
}


void MainWindow::on_revButton_clicked()
{
    QString num = ui->textbox->text();
    if (num.size() == 0) return;
    ui->textbox->setText("");
    forPrintTree->setText("");
    ui->history->setText(">>>Removing key " + num + "...");
    QApplication::processEvents();
    bool succ = renderArea->remove(convertToNum(num));
    renderArea->repaint();
    if (succ)
    {
        ui->history->setText(">>>Key " + num + " was removed");
    }
    else
    {
        ui->history->setText(">>>Key " + num + " does not exist in the tree");
        QMessageBox::critical(this,"Search","Key " + num + " does not exist in the tree");
    }
}


void MainWindow::on_clrButton_clicked()
{
    forPrintTree->setText("");
    renderArea->clear();
    renderArea->repaint();
    ui->history->setText(">>>Screen Cleared");
}

bool MainWindow::isNum(const QString& st)
{
    for (int i = 0;i < sz(st); ++i)
        if (!st[i].isDigit()) return false;
    return true;
}

int MainWindow::convertToNum(QString st)
{
    int res = 0;
    for (int i=0;i<sz(st);++i)
        res = res * 10 + (st[i].digitValue());
    return res;
}

QString MainWindow::convertToString(int x)
{
    QString res = "";
    while (x)
    {
        res = char(x % 10 + '0') + res;
        x /= 10;
    }
    return res;
}
void MainWindow::on_slider_valueChanged(int value)
{
    renderArea->SetSliderVal(value);
}


void MainWindow::on_printButton_clicked()
{
    QString printTree = renderArea->print();
    if (printTree.size() == 0)
        ui->history->setText(">>>The tree is empty. Nothing to do here");
    else
    {
        ui->history->setText(">>>B-Tree was printed");
        forPrintTree->setText(printTree);
    }
}

void MainWindow::on_maxDegOpt_currentTextChanged(const QString &arg1)
{
    if (renderArea == NULL) return;
    int curMaxDeg = convertToNum(ui->maxDegOpt->currentText());
    //qDebug() << curMaxDeg << '\n';
    renderArea->SetMaxDeg(curMaxDeg);
    //qDebug() << "?\n";
    renderArea->repaint();
}
