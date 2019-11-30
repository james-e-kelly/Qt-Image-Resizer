#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    processButton(nullptr),
    fileList(nullptr),
    newWidth(nullptr),
    addFolderButton(nullptr),
    removeFilesButton(nullptr),
    addFilesButton(nullptr),
    debugList(nullptr),
    sixtyFour(nullptr),
    hundredTwentyEight(nullptr),
    twoHundred(nullptr),
    fiveHundred(nullptr),
    customWidth(nullptr)
{
    ui->setupUi(this);

    setWindowTitle("Image Resizer");

    // Move Mac tool bar to widget
    setUnifiedTitleAndToolBarOnMac(true);

    // Set size and fix size
    setFixedSize(size());
    setWindowFlags(Qt::WindowType::Window | Qt::MSWindowsFixedSizeDialogHint);

    // ----- References -----
    addFolderButton = ui->AddFolderButton;
    fileList = ui->FileList;
    removeFilesButton = ui->RemoveFileButton;
    addFilesButton = ui->AddFileButton;
    newWidth = ui->NewWidth;
    processButton = ui->ConfirmButton;
    debugList = ui->DebugList;
    sixtyFour = ui->SixtyFour;
    hundredTwentyEight = ui->HundredTwentyEight;
    twoHundred = ui-> TwoHundred;
    fiveHundred = ui->FiveHundred;
    customWidth = ui->CustomWidth;

    // Slots
    connect(addFilesButton, SIGNAL(released()), this, SLOT(AddFile()));
    connect(addFolderButton, SIGNAL(released()), this, SLOT(AddFolder()));
    connect(removeFilesButton, SIGNAL(released()), this, SLOT(RemoveFileInList()));
    connect(processButton, SIGNAL(released()), this, SLOT(Process()));
    connect(ui->ClearFiles, SIGNAL(released()), this, SLOT(ClearFiles()));
    connect(ui->ClearDebug, SIGNAL(released()), this, SLOT(ClearDebug()));

    // Allow drag and drop
    setAcceptDrops(true);
}

MainWindow::~MainWindow()
{
    delete ui;
}

/// DIALOGUE: Add a selection of files, ignoring folders
void MainWindow::AddFile()
{
    QStringList files = QFileDialog::getOpenFileNames(this, "Select Files", "/home", "*.png *.jpeg *.jpg");

    for (QString filePath : files)
    {
        AddFileToList(filePath);
    }
}

/// DIALOGUE: Add a folder recursively
void MainWindow::AddFolder()
{
    QString dir = QFileDialog::getExistingDirectory(this, "Select Folder", "/home", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    AddFolderToList(dir);
}


/// BUTTON: Removes the selected items in the list
void MainWindow::RemoveFileInList()
{
    QList<QListWidgetItem *> selectedItems(fileList->selectedItems());

    if (selectedItems.length() > 0)
    {
        for (QListWidgetItem * item : selectedItems)
        {
            delete item;
        }
    }
}

/// DRAG AND DROP: Allows drag and drop functionality if the selected items are valid extensions or a folder
void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls()) {
        for (const QUrl url : event->mimeData()->urls())
        {
            QString fileName = url.toLocalFile();
            if (!IsDir(fileName))
                if (!IsValidFile(fileName)) return;
        }
        event->acceptProposedAction();
    }
}

/// DRAG AND DROP: Adds all images in a folder or adds all selected images
void MainWindow::dropEvent(QDropEvent *event)
{
    foreach (const QUrl &url, event->mimeData()->urls()) {
            QString fileName = url.toLocalFile();
            if (IsDir(fileName))
            {
                AddFolderToList(fileName);
            }
            else
            {
                AddFileToList(fileName);
            }
        }
}

/// DRAG AND DROP: Allowing functionality to work
void MainWindow::dragMoveEvent(QDragMoveEvent *event)
{
    event->acceptProposedAction();
}

/// BUTTON: Process all images in the list
void MainWindow::Process()
{
    // Main setup and error checking
    if (fileList->count() == 0) return;
    int newWidthValue = -1;
    bool parseOkay = false;
    if (customWidth->isChecked())
    {
        newWidthValue = newWidth->text().toInt(&parseOkay);
    }


    qDebug() << "Processing";

    PrintDebugString("Resizing images...");

    // Everything should be fine so do the resizing

    for (int i = 0; i < fileList->count(); i++)
    {
        // Get item from list
        QListWidgetItem *item = fileList->item(i);

        // Get file info
        QFileInfo info (item->text());

        // Create new file name
//        QString newFileName(CreateNewFileString(info.filePath(), newWidthValue));

//        PrintDebugString("Resizing \"" + info.filePath() + "\"");

        QImage origImage(info.filePath());

        if (sixtyFour->isChecked())
        {
            ScaleImage(64, &origImage, &info);
        }
        if (hundredTwentyEight->isChecked())
        {
            ScaleImage(128, &origImage, &info);
        }
        if (twoHundred->isChecked())
        {
            ScaleImage(256, &origImage, &info);
        }
        if (fiveHundred->isChecked())
        {
            ScaleImage(512, &origImage, &info);
        }
        if (customWidth->isChecked() && parseOkay && newWidthValue != -1)
        {
            ScaleImage(newWidthValue, &origImage, &info);
        }
    }
    PrintDebugString("Done");
    PrintDebugString("");
}

/// PROCESS: If the image hasn't already been scaled to this size, will scale the image and create a new file
void MainWindow::ScaleImage(int newWidth, QImage *originalImage, QFileInfo *info)
{
    QString newFileName(CreateNewFileString(info->filePath(), newWidth));
    if (FileExists(newFileName))
    {
        PrintDebugString("File Already Exists: \"" + info->filePath() + "\"");
        return;
    }
    if (originalImage->width() <= newWidth && !ui->Upscaling->isChecked())
    {
        PrintDebugString("File Too Small: \"" + info->filePath() + "\". Upscaling is turned off");
        return;
    }

    QPixmap pixmap = pixmap.fromImage(originalImage->scaledToWidth(newWidth, Qt::SmoothTransformation));
    QFile newFile(newFileName);

    newFile.open(QIODevice::WriteOnly);
    pixmap.save(&newFile, "png", 100);
    newFile.close();

    PrintDebugString("Resized \"" + newFileName + "\" to " + QString::number(newWidth) + " pixels wide");
    qDebug() << "Created new image: " + newFileName;
}

void MainWindow::AddFileToList(QString file)
{

    if (!FileExistsInList(file))
    {
        PrintDebugString("Adding \"" + file + "\"");
        fileList->addItem(file);
    }
    else
    {
        PrintDebugString("\"" + file + "\"already in the list");
    }
}

/// INTERNAL: Creates a new file name and path with the width appended to the filename
QString MainWindow::CreateNewFileString(QFileInfo origFileInfo, int width)
{
    return origFileInfo.path() + "/" + QString::number(width) + "_" + origFileInfo.fileName();
}

/// INTERNAL: Returns true if the file exists
bool MainWindow::FileExists(QString fileString)
{
    return QFile::exists(fileString);
}

/// INTERNAL: Returns true if the path is a directory/folder
bool MainWindow::IsDir(QString path)
{
    QFileInfo info(path);
    if (info.isDir()) {
        return true;
    }
    return false;
}

/// INTERNAL: Add images from a folder recursively
void MainWindow::AddFolderToList(QString dir)
{
    QDirIterator it (dir, QStringList() << "*.png" << "*.jpg" << ".jpeg", QDir::Files, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        QString nextFile = it.next();
        if (IsValidFile(nextFile)) {
            AddFileToList(nextFile);
        }
    }
}


/// INTERNAL: Returns true if the file is a .png or .jpg
bool MainWindow::IsValidFile(QString path)
{
    QFileInfo info(path);
    if (info.suffix() != "png" && info.suffix() != "jpg") {
        return false;
    }
    return true;
}

bool MainWindow::FileExistsInList(QString file)
{
    for (int i = 0; i < fileList->count(); i++)
    {
        if (fileList->item(i)->text() == file)
        {
            return  true;
        }
    }
    return false;
}

/// DEBUG: Sends a message to the debug list on the UI
void MainWindow::PrintDebugString(QString debugMessage)
{
    debugList->addItem(debugMessage);
}

void MainWindow::ClearFiles()
{
    fileList->clear();
}

void MainWindow::ClearDebug()
{
    debugList->clear();
}
