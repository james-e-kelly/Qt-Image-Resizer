#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDebug>
#include <QLoggingCategory>
#include <QWidget>
#include <QDropEvent>
#include <QDragMoveEvent>
#include <QDragEnterEvent>
#include <QMainWindow>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QListWidget>
#include <QToolButton>
#include <QFileDialog>
#include <QCheckBox>
#include <QDirIterator>
#include <QUrl>
#include <QMimeData>
#include <QImage>
#include <QPixmap>
#include <QFile>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;

private:
    Ui::MainWindow *ui;

    QPushButton *processButton;

    QListWidget *fileList;

    QLineEdit *newWidth;

    QPushButton *addFolderButton;
    QToolButton *removeFilesButton;
    QToolButton *addFilesButton;

    QListWidget *debugList;

    QCheckBox *sixtyFour;
    QCheckBox *hundredTwentyEight;
    QCheckBox *twoHundred;
    QCheckBox *fiveHundred;
    QCheckBox *customWidth;

    void AddFileToList(QString file);
    void AddFolderToList(QString dir);
    bool IsValidFile (QString path);
    bool IsDir (QString path);
    QString CreateNewFileString(QFileInfo origFileInfo, int width);
    bool FileExists(QString fileString);
    void PrintDebugString(QString debugMessage);
    bool FileExistsInList(QString file);
    void ScaleImage (int newWidth, QImage *originalImage, QFileInfo *info);

private slots:
    void AddFile(void);
    void AddFolder(void);
    void RemoveFileInList(void);
    void Process(void);
    void ClearFiles(void);
    void ClearDebug(void);
};

#endif // MAINWINDOW_H
