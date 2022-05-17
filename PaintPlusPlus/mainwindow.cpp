#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtDebug>
#include <QColorDialog>

using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->canvasLabel, SIGNAL(sendMousePosition(int&, int&)), this, SLOT(mouseMove(int&, int&)));
    connect(ui->canvasLabel, SIGNAL(sendMouseClicked(int&, int&)), this, SLOT(mouseIsPressed(int&, int&)));
    connect(ui->canvasLabel, SIGNAL(sendMouseReleased(int&, int&)), this, SLOT(mouseIsReleased(int&, int&)));
    color = rgbToHex(0,0,0);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::receiveCanvas(int& x, int& y) {
    qDebug() << x << y;
    createNewCanvas(x, y);
}


void MainWindow::createNewCanvas(int x, int y)
{
    ui->canvasLabel->setEnabled(true);
    ui->canvasLabel->createNewMatrix(x, y);
    this->tools.setMatrixPointer(ui->canvasLabel->getMatrix());

    this->imageDimensions[0] = x;
    this->imageDimensions[1] = y;

    ui->gridLayout->setGeometry(QRect(x, y, x, y));
    fillCanvas(255, 255, 255, x, y);
}


uint32_t MainWindow::rgbToHex(int r, int g, int b) {
    return ((r & 0xFF) << 16) + ((g & 0xFF) << 8) + ((b & 0xFF));
}

void MainWindow::fillCanvas(int r, int g, int b, int x, int y)
{
    QImage *image = new QImage(x, y, QImage::Format_RGB32);
    uint32_t color = rgbToHex(r, g, b);

    for (int xi = 0; xi < x; xi++) {
        for (int yi = 0; yi < y; yi++) {
            image->setPixel(xi, yi, color);
        }
    }

    ui->canvasLabel->setPixmap(QPixmap::fromImage(*image));
    delete image;
}

void MainWindow::mouseIsPressed(int& x, int& y)
{
    this->mousePressedInCanvas = true;
    this->firstClick[0] = x;
    this->firstClick[1] = y;

    if (this->penF) {
       setPixelInCanvas(x, y);
    }
}

void MainWindow::mouseIsReleased(int& x, int& y)
{
    this->mousePressedInCanvas = false;

    this->clickReleased[0] = x;
    this->clickReleased[1] = y;

    if (this->pencilF) {
        drawALine(this->firstClick, this->clickReleased);
    }
    if (this->squareF){
        drawSquare(this->firstClick, this->clickReleased);
    }
    if(this->circleF){
        drawCircle(this->firstClick, this->clickReleased);
    }
}

void MainWindow::mouseMove(int &x, int &y)
{
    std::string location = std::to_string(x) + ", " + std::to_string(y);
    ui->mouseLocationLabel->setText(QString::fromStdString(location));

    setPixelInCanvas(x, y);
}

void MainWindow::updateCanvas() {
    int x = this->imageDimensions[0];
    int y = this->imageDimensions[1];
    pixel **matrix = ui->canvasLabel->getMatrix();
    QImage canvas = ui->canvasLabel->pixmap().toImage();
    for (int i = 0; i < x; i++) {
        for (int j = 0; j < y; j++) {
            canvas.setPixel(i, j, matrix[i][j].getColor());
        }
    }
    ui->canvasLabel->setPixmap(QPixmap::fromImage(canvas));
}

void MainWindow::drawALine(int start[], int end[]) {
    tools.drawWithPencil(start, end, this->color,ui->spinBox->value());
    updateCanvas();
}

void MainWindow::drawSquare(int *start, int *end){
    tools.drawSquare(start, end, this->color,ui->spinBox->value());
    updateCanvas();
}

void MainWindow::drawCircle(int *start, int *end){
    tools.drawCircle(start, end, this->color,ui->spinBox->value());
    updateCanvas();
}

void MainWindow::setPixelInCanvas(int x, int y) {
    if (this->penF && this->mousePressedInCanvas) {
        QImage canvas = ui->canvasLabel->pixmap().toImage();
        int grosor = ui->spinBox->value();

        for (int i = -grosor; i < grosor; i++){
            for(int j = -grosor; j < grosor; j++){
                 canvas.setPixel(x + i , y +j, this->color);
                 tools.drawWithPen(x + i, y + j, this->color);
            }

        }
        ui->canvasLabel->setPixmap(QPixmap::fromImage(canvas));
    }
}

void MainWindow::on_actionNuevo_triggered()
{
    createcanvasDialog = new createCanvasDialog();
    connect(createcanvasDialog, SIGNAL(accept(int&, int&)), this, SLOT(receiveCanvas(int&, int&)));
    createcanvasDialog->setModal(true);
    createcanvasDialog->exec();

    on_penButton_clicked();
}




void MainWindow::falseAllTools()
{
    this->penF = false;
    this->pencilF = false;
    this->squareF = false;
    this->circleF = false;
}

void MainWindow::trueAllButtons() {
    ui->penButton->setEnabled(true);
    ui->pencilButton->setEnabled(true);
    ui->squareButton->setEnabled(true);
    ui->circle_Button->setEnabled(true);
}

void MainWindow::on_penButton_clicked()
{
    falseAllTools();
    this->penF = true;
    trueAllButtons();
    ui->penButton->setEnabled(false);
}

void MainWindow::on_pencilButton_clicked()
{
    falseAllTools();
    this->pencilF = true;
    trueAllButtons();
    ui->pencilButton->setEnabled(false);
}

void MainWindow::on_squareButton_clicked()
{
    falseAllTools();
    this->squareF = true;
    trueAllButtons();
    ui->squareButton->setEnabled(false);
}

void MainWindow::on_circle_Button_clicked()
{
    falseAllTools();
    this->circleF = true;
    trueAllButtons();
    ui->circle_Button->setEnabled(false);
}

void MainWindow::on_Color_button_clicked()
{
    QColor code = QColorDialog::getColor(Qt::blue,this, "Select Color", QColorDialog::DontUseNativeDialog);
    int red = code.red();
    int green = code.green();
    int blue = code.blue();
    this->color = rgbToHex(red, green, blue);

}

void MainWindow::on_pushButton_3_clicked()
{
    this->updateCanvas();
}





void MainWindow::on_pushButton_2_clicked()
{

}


void MainWindow::on_pushButton_clicked()
{

}






