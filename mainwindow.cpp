/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

/**
 * @brief MainWindos class
 * @fn mainwindow.cpp
 * @version V1.1.0
 * @author shenxfsn@@163.com
 */
#include <QtWidgets>
#include <QFileInfo>
#include "mainwindow.h"
#include "qtpropertybrowser/qttreepropertybrowser.h"
#include "qtpropertybrowser/qtvariantproperty.h"

/**
 * @brief MainWindow::MainWindow
 */
MainWindow::MainWindow()
{
    outPath = QDir::homePath()+tr("/kyj1");
    coffPath = QDir::homePath()+tr("/kyj1.out");
    symbol_qk = QString(tr("_temperature_qk"));
    symbol_qb = QString(tr("_temperature_qb"));
    if(QFileInfo(coffPath).exists())
    {
        coff.open(coffPath);
    }
    isSaved =false;
    init();
    setCurrentFile(QString());
}

/**
 * @brief MainWindow::MainWindow
 * @param fileName
 */
MainWindow::MainWindow(const QString &fileName)
{
    init();
    loadFile(fileName);
}

/**
 * @brief MainWindow::closeEvent
 * @param event
 */
void MainWindow::closeEvent(QCloseEvent *event)
{
    if (maybeSave()) {
        writeSettings();
        event->accept();
    } else {
        event->ignore();
    }
}

/**
 * @brief MainWindow::newFile
 */
void MainWindow::newFile()
{
    MainWindow *other = new MainWindow;
    other->tile(this);
    other->show();
}

/**
 * @brief MainWindow::open
 */
void MainWindow::open()
{
    const QString fileName = QFileDialog::getOpenFileName(this);
    if (!fileName.isEmpty())
        openFile(fileName);
}

/**
 * @brief MainWindow::openFile
 * @param fileName
 */
void MainWindow::openFile(const QString &fileName)
{
    MainWindow *existing = findMainWindow(fileName);
    if (existing) {
        existing->show();
        existing->raise();
        existing->activateWindow();
        return;
    }

    if (isUntitled && textEdit->document()->isEmpty() && !isWindowModified()) {
        loadFile(fileName);
        return;
    }

    MainWindow *other = new MainWindow(fileName);
    if (other->isUntitled) {
        delete other;
        return;
    }
    other->tile(this);
    other->show();
}

/**
 * @brief MainWindow::save
 * @return
 */
bool MainWindow::save()
{
    return isUntitled ? saveAs() : saveFile(curFile);
}

/**
 * @brief MainWindow::saveAs
 * @return
 */
bool MainWindow::saveAs()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("另存"),
                                                    curFile);
    if (fileName.isEmpty())
        return false;

    return saveFile(fileName);
}

/**
 * @brief MainWindow::about
 */
void MainWindow::about()
{
   QMessageBox::about(this, tr("关于KYJ1gen"),
            tr("<b>KYJ1gen</b> 监控管理软件固件生成工具，"
               "基于Qt SDI示例工程。版本V1.1.0"));
}

/**
 * @brief MainWindow::documentWasModified
 */
void MainWindow::documentWasModified()
{
    setWindowModified(true);
}

/**
 * @brief MainWindow::init
 */
void MainWindow::init()
{
    setAttribute(Qt::WA_DeleteOnClose);

    isUntitled = true;

    textEdit = new QTextEdit;
    textEdit->setText(tr("[Batch]\r\nS1\r\n[Product]\r\n201801 201801\r\n0.018632 1.123\r\n"));
    textEdit->document()->setModified();

    setCentralWidget(textEdit);

    createActions();
    createStatusBar();
    creatDockWidget();
    readSettings();

    connect(textEdit->document(), &QTextDocument::contentsChanged,
            this, &MainWindow::documentWasModified);

    setUnifiedTitleAndToolBarOnMac(true);
}

/**
 * @brief MainWindow::tile
 * @param previous
 */
void MainWindow::tile(const QMainWindow *previous)
{
    if (!previous)
        return;
    int topFrameWidth = previous->geometry().top() - previous->pos().y();
    if (!topFrameWidth)
        topFrameWidth = 40;
    const QPoint pos = previous->pos() + 2 * QPoint(topFrameWidth, topFrameWidth);
    if (QApplication::desktop()->availableGeometry(this).contains(rect().bottomRight() + pos))
        move(pos);
}


//! [implicit tr context]
/**
 * @brief MainWindow::createActions
 */
void MainWindow::createActions()
{
    QMenu *fileMenu = menuBar()->addMenu(tr("文件"));
//! [implicit tr context]
    QToolBar *fileToolBar = addToolBar(tr("文件"));

    const QIcon newIcon = QIcon::fromTheme("document-new", QIcon(":/images/new.png"));
    QAction *newAct = new QAction(newIcon, tr("新建"), this);
    newAct->setShortcuts(QKeySequence::New);
    newAct->setStatusTip(tr("新建传感器数据文件"));
    connect(newAct, &QAction::triggered, this, &MainWindow::newFile);
    fileMenu->addAction(newAct);
    fileToolBar->addAction(newAct);

    const QIcon openIcon = QIcon::fromTheme("document-open", QIcon(":/images/open.png"));
    QAction *openAct = new QAction(openIcon, tr("打开..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("打开已存在的传感器数据文件"));
    connect(openAct, &QAction::triggered, this, &MainWindow::open);
    fileMenu->addAction(openAct);
    fileToolBar->addAction(openAct);

    const QIcon saveIcon = QIcon::fromTheme("document-save", QIcon(":/images/save.png"));
    QAction *saveAct = new QAction(saveIcon, tr("保存"), this);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip(tr("保存传感器数据文件"));
    connect(saveAct, &QAction::triggered, this, &MainWindow::save);
    fileMenu->addAction(saveAct);
    fileToolBar->addAction(saveAct);

    const QIcon saveAsIcon = QIcon::fromTheme("document-save-as");
    QAction *saveAsAct = fileMenu->addAction(saveAsIcon, tr("另存..."), this, &MainWindow::saveAs);
    saveAsAct->setShortcuts(QKeySequence::SaveAs);
    saveAsAct->setStatusTip(tr("换名保存传感器数据文件"));

    fileMenu->addSeparator();

    QMenu *recentMenu = fileMenu->addMenu(tr("最近打开文件..."));
    connect(recentMenu, &QMenu::aboutToShow, this, &MainWindow::updateRecentFileActions);
    recentFileSubMenuAct = recentMenu->menuAction();

    for (int i = 0; i < MaxRecentFiles; ++i) {
        recentFileActs[i] = recentMenu->addAction(QString(), this, &MainWindow::openRecentFile);
        recentFileActs[i]->setVisible(false);
    }

    recentFileSeparator = fileMenu->addSeparator();

    setRecentFilesVisible(MainWindow::hasRecentFiles());

    QAction *closeAct = fileMenu->addAction(tr("&关闭"), this, &QWidget::close);
    closeAct->setShortcut(tr("Ctrl+W"));
    closeAct->setStatusTip(tr("关闭窗口"));

    const QIcon exitIcon = QIcon::fromTheme("application-exit");
    QAction *exitAct = fileMenu->addAction(exitIcon, tr("E&xit"), qApp, &QApplication::closeAllWindows);
    exitAct->setShortcuts(QKeySequence::Quit);
    exitAct->setStatusTip(tr("Exit the application"));

    QMenu *editMenu = menuBar()->addMenu(tr("编辑"));
    QToolBar *editToolBar = addToolBar(tr("编辑"));

#ifndef QT_NO_CLIPBOARD
    const QIcon cutIcon = QIcon::fromTheme("edit-cut", QIcon(":/images/cut.png"));
    QAction *cutAct = new QAction(cutIcon, tr("剪切"), this);
    cutAct->setShortcuts(QKeySequence::Cut);
    cutAct->setStatusTip(tr("剪切已选内容至剪切板"));
    connect(cutAct, &QAction::triggered, textEdit, &QTextEdit::cut);
    editMenu->addAction(cutAct);
    editToolBar->addAction(cutAct);

    const QIcon copyIcon = QIcon::fromTheme("edit-copy", QIcon(":/images/copy.png"));
    QAction *copyAct = new QAction(copyIcon, tr("复制"), this);
    copyAct->setShortcuts(QKeySequence::Copy);
    copyAct->setStatusTip(tr("复制已选内容至剪切板"));
    connect(copyAct, &QAction::triggered, textEdit, &QTextEdit::copy);
    editMenu->addAction(copyAct);
    editToolBar->addAction(copyAct);

    const QIcon pasteIcon = QIcon::fromTheme("edit-paste", QIcon(":/images/paste.png"));
    QAction *pasteAct = new QAction(pasteIcon, tr("粘贴"), this);
    pasteAct->setShortcuts(QKeySequence::Paste);
    pasteAct->setStatusTip(tr("将剪切板内容内容粘贴至已选段落"));
    connect(pasteAct, &QAction::triggered, textEdit, &QTextEdit::paste);
    editMenu->addAction(pasteAct);
    editToolBar->addAction(pasteAct);

    menuBar()->addSeparator();
#endif // !QT_NO_CLIPBOARD

#ifndef QT_NO_CLIPBOARD
    cutAct->setEnabled(false);
    copyAct->setEnabled(false);
    connect(textEdit, &QTextEdit::copyAvailable, cutAct, &QAction::setEnabled);
    connect(textEdit, &QTextEdit::copyAvailable, copyAct, &QAction::setEnabled);
#endif // !QT_NO_CLIPBOARD
    viewMenu = menuBar()->addMenu(tr("视图"));
    QMenu *chipmenu = menuBar()->addMenu(tr("固件"));
    QToolBar *chipToolBar = addToolBar(tr("固件"));
    const QIcon  settingIcon = QIcon::fromTheme("setting",QIcon(":/images/setting.png"));
    QAction *settingAct = new QAction(settingIcon,tr("设置"),this);
    settingAct->setStatusTip(tr("设置固件路径和输出路径"));
    connect(settingAct,&QAction::triggered,this,&MainWindow::setting);
    chipmenu->addAction(settingAct);
    chipToolBar->addAction(settingAct);

    const QIcon runIcon = QIcon::fromTheme("running",QIcon(":/images/chip.png"));
    QAction *runAct = new QAction(runIcon,tr("生成"),this);
    runAct->setStatusTip(tr("生成固件"));
    connect(runAct,&QAction::triggered,this,&MainWindow::generate);
    chipmenu->addAction(runAct);
    chipToolBar->addAction(runAct);

    QMenu *helpMenu = menuBar()->addMenu(tr("帮助"));
    const QIcon helpIcon = QIcon::fromTheme("help", QIcon(":/images/help.png"));
    QAction *aboutAct = helpMenu->addAction(helpIcon,tr("关于"), this, &MainWindow::about);
    aboutAct->setStatusTip(tr("显示关于对话框"));
    chipToolBar->addAction(aboutAct);

    QAction *aboutQtAct = helpMenu->addAction(tr("关于Qt"), qApp, &QApplication::aboutQt);
    aboutQtAct->setStatusTip(tr("显示关于Qt库对话框"));

}

/**
 * @brief MainWindow::createStatusBar
 */
void MainWindow::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
}

/**
 * @brief MainWindow::readSettings
 */
void MainWindow::readSettings()
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    const QByteArray geometry = settings.value("geometry", QByteArray()).toByteArray();
    if (geometry.isEmpty()) {
        const QRect availableGeometry = QApplication::desktop()->availableGeometry(this);
        resize(availableGeometry.width() / 3, availableGeometry.height() / 2);
        move((availableGeometry.width() - width()) / 2,
             (availableGeometry.height() - height()) / 2);
    } else {
        restoreGeometry(geometry);
    }
}

/**
 * @brief MainWindow::writeSettings
 */
void MainWindow::writeSettings()
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    settings.setValue("geometry", saveGeometry());
}

/**
 * @brief MainWindow::maybeSave
 * @return
 */
bool MainWindow::maybeSave()
{
    if (!textEdit->document()->isModified())
        return true;
    const QMessageBox::StandardButton ret
        = QMessageBox::warning(this, tr("KYJ1固件生成工具"),
                               tr("传感器数据文件已改变，要保存吗？"),
                               QMessageBox::Save | QMessageBox::Discard
                               | QMessageBox::Cancel);
    switch (ret) {
    case QMessageBox::Save:
        return save();
    case QMessageBox::Cancel:
        return false;
    default:
        break;
    }
    return true;
}

/**
 * @brief MainWindow::loadFile
 * @param fileName
 */
void MainWindow::loadFile(const QString &fileName)
{

    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("KYJ1固件生成工具"),
                             tr("不能读取传感器数据文件 %1:\n%2.")
                             .arg(QDir::toNativeSeparators(fileName), file.errorString()));
        return;
    }

    QTextStream in(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    textEdit->setPlainText(in.readAll());
    QApplication::restoreOverrideCursor();

    setCurrentFile(fileName);
    statusBar()->showMessage(tr("File loaded"), 2000);
}

/**
 * @brief MainWindow::setRecentFilesVisible
 * @param visible
 */
void MainWindow::setRecentFilesVisible(bool visible)
{
    recentFileSubMenuAct->setVisible(visible);
    recentFileSeparator->setVisible(visible);
}

/**
 * @brief recentFilesKey
 * @return
 */
static inline QString recentFilesKey() { return QStringLiteral("recentFileList"); }

/**
 * @brief fileKey
 * @return
 */
static inline QString fileKey() { return QStringLiteral("file"); }

/**
 * @brief readRecentFiles
 * @param settings
 * @return
 */
static QStringList readRecentFiles(QSettings &settings)
{
    QStringList result;
    const int count = settings.beginReadArray(recentFilesKey());
    for (int i = 0; i < count; ++i) {
        settings.setArrayIndex(i);
        result.append(settings.value(fileKey()).toString());
    }
    settings.endArray();
    return result;
}

/**
 * @brief writeRecentFiles
 * @param files
 * @param settings
 */
static void writeRecentFiles(const QStringList &files, QSettings &settings)
{
    const int count = files.size();
    settings.beginWriteArray(recentFilesKey());
    for (int i = 0; i < count; ++i) {
        settings.setArrayIndex(i);
        settings.setValue(fileKey(), files.at(i));
    }
    settings.endArray();
}

/**
 * @brief MainWindow::hasRecentFiles
 * @return
 */
bool MainWindow::hasRecentFiles()
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    const int count = settings.beginReadArray(recentFilesKey());
    settings.endArray();
    return count > 0;
}

/**
 * @brief MainWindow::prependToRecentFiles
 * @param fileName
 */
void MainWindow::prependToRecentFiles(const QString &fileName)
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());

    const QStringList oldRecentFiles = readRecentFiles(settings);
    QStringList recentFiles = oldRecentFiles;
    recentFiles.removeAll(fileName);
    recentFiles.prepend(fileName);
    if (oldRecentFiles != recentFiles)
        writeRecentFiles(recentFiles, settings);

    setRecentFilesVisible(!recentFiles.isEmpty());
}

/**
 * @brief MainWindow::updateRecentFileActions
 */
void MainWindow::updateRecentFileActions()
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());

    const QStringList recentFiles = readRecentFiles(settings);
    const int count = qMin(int(MaxRecentFiles), recentFiles.size());
    int i = 0;
    for ( ; i < count; ++i) {
        const QString fileName = MainWindow::strippedName(recentFiles.at(i));
        recentFileActs[i]->setText(tr("&%1 %2").arg(i + 1).arg(fileName));
        recentFileActs[i]->setData(recentFiles.at(i));
        recentFileActs[i]->setVisible(true);
    }
    for ( ; i < MaxRecentFiles; ++i)
        recentFileActs[i]->setVisible(false);
}

/**
 * @brief MainWindow::openRecentFile
 */
void MainWindow::openRecentFile()
{
    if (const QAction *action = qobject_cast<const QAction *>(sender()))
        openFile(action->data().toString());
}

/**
 * @brief MainWindow::saveFile
 * @param fileName
 * @return
 */
bool MainWindow::saveFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("SDI"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(QDir::toNativeSeparators(fileName), file.errorString()));
        return false;
    }

    QTextStream out(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    out << textEdit->toPlainText();
    QApplication::restoreOverrideCursor();

    setCurrentFile(fileName);
    statusBar()->showMessage(tr("File saved"), 2000);
    return true;
}

/**
 * @brief MainWindow::setCurrentFile
 * @param fileName
 */
void MainWindow::setCurrentFile(const QString &fileName)
{
    static int sequenceNumber = 1;

    isUntitled = fileName.isEmpty();
    if (isUntitled) {
        curFile = tr("document%1.txt").arg(sequenceNumber++);
    } else {
        curFile = QFileInfo(fileName).canonicalFilePath();
    }

    textEdit->document()->setModified(false);
    setWindowModified(false);

    if (!isUntitled && windowFilePath() != curFile)
        MainWindow::prependToRecentFiles(curFile);

    setWindowFilePath(curFile);
}

/**
 * @brief MainWindow::strippedName
 * @param fullFileName
 * @return
 */
QString MainWindow::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

MainWindow *MainWindow::findMainWindow(const QString &fileName) const
{
    QString canonicalFilePath = QFileInfo(fileName).canonicalFilePath();

    foreach (QWidget *widget, QApplication::topLevelWidgets()) {
        MainWindow *mainWin = qobject_cast<MainWindow *>(widget);
        if (mainWin && mainWin->curFile == canonicalFilePath)
            return mainWin;
    }

    return 0;
}

/**
 * @brief MainWindow::creatDockWidget
 */
void MainWindow::creatDockWidget()
{
    QDockWidget *dock = new QDockWidget(tr("固件输出"), this);
    //dock->setAllowedAreas(Qt::BottomDockWidgetArea);
    output = new QTextEdit(dock);
    output->setText(
             tr("KYJ1监控管理软件固件生成工具，V1.0.1\n"
               "KYJ1监控管理软件默认可执行目标文件是")
             + QFileInfo(coffPath).fileName()
             + tr("，该文件应存在Home(")
             + QFileInfo(coffPath).absolutePath()
             + tr(")目录中，可在“固件”菜单的“设置”中指定\n"
                 "生成固件的默认目录是：Home的kyj1子目录(")
             + QFileInfo(outPath).absolutePath()
             + tr("/kyj1)，也可在“固件”菜单的“设置”中指定\n"
                  "默认Qk标识符：temperature_qk,Qb标识符:temperature_qb，"
                  "也可在“固件“菜单的“设置”中指定\n"));
    output->setReadOnly(true);
    dock->setWidget(output);
    addDockWidget(Qt::BottomDockWidgetArea,dock);
    viewMenu->addAction(dock->toggleViewAction());

    QDockWidget *dock1 = new QDockWidget(tr("COFF文件属性"), this);
    coffInfo = new QtTreePropertyBrowser(dock1);
    m_pVarManager = new QtVariantPropertyManager(coffInfo);

    QDockWidget *dock2 = new QDockWidget(tr("段属性"));
    sectionInfo = new QtTreePropertyBrowser(dock2);
    m_pVarManager1 = new QtVariantPropertyManager(sectionInfo);

    QDockWidget *dock3 = new QDockWidget(tr("符号表"));
    symWidget = new QTableWidget(dock3);

    symWidget->setColumnCount(6);

    coffProperty();
    dock1->setWidget(coffInfo);
    addDockWidget(Qt::RightDockWidgetArea,dock1);
    viewMenu->addAction(dock1->toggleViewAction());
    dock2->setWidget(sectionInfo);
    addDockWidget(Qt::RightDockWidgetArea,dock2);
    viewMenu->addAction(dock2->toggleViewAction());
    dock3->setWidget(symWidget);
    addDockWidget(Qt::BottomDockWidgetArea,dock3);
    viewMenu->addAction(dock3->toggleViewAction());
}

/**
 * @brief MainWindow::setting
 */
void MainWindow::setting()
{
    Setting *settingWidget = new Setting(this);
    settingWidget->setModal(true);

    settingWidget->show();

    if(settingWidget->exec())
    {
        if(coffPath != settingWidget->coffPath_sub)
        {
            coffPath = settingWidget->coffPath_sub;
            coff.open(coffPath);
            coffProperty();
        }
        if(outPath != settingWidget->outPath_sub)
        {
            outPath = settingWidget->outPath_sub;
        }
        if(symbol_qk.mid(1) == settingWidget->symQk)
        {
            symbol_qk = QString(tr("_")) + settingWidget->symQk;
        }
        if(symbol_qb.mid(1) == settingWidget->symQb)
        {
            symbol_qb = QString(tr("_")) + settingWidget->symQb;
        }
        output->append(QString(tr("OUT文件名：")) + QFileInfo(coffPath).fileName());
        output->append(QString(tr("固件输出目录：")) + outPath);
    }
}

/**
 * @brief MainWindow::generate
 */
void MainWindow::generate()
{
    QCursor cs = cursor();
    setCursor(Qt::WaitCursor);
    if (textEdit->document()->isModified() | isUntitled)
    {
        const QMessageBox::StandardButton ret
            = QMessageBox::warning(this, tr("KYJ1gen"),
                               tr("传感器数据文件已改变，要保存吗？"),
                               QMessageBox::Save | QMessageBox::Discard
                               | QMessageBox::Cancel);
        if(ret == QMessageBox::Save)
        {
             save();
        }
    }

    if((coff.FindSymbol(symbol_qk) == NULL)||(coff.FindSymbol(symbol_qb) == NULL))
    {
       output->append(QString(tr("OUT文件不含有Qk、Qb标识符，请选择正确的OUT文件，"
                                 "生成固件终止\n")));
       return;
    }

    productList.clear();
    QDir dir(outPath);
    if(!dir.exists())
    {
        dir.mkdir(outPath);
        output->append(QString(tr("创建%1目录").arg(outPath)));
    }

    output->append(tr("开始生成固件"));
    QString strText = textEdit->toPlainText();
    QString strline;
    QString batch(tr("S1"));
    QStringList sec;
    QStringList pnList;
    QTextStream ts(&strText);
    quint32 lineNum = 0;
    quint8 status = 0;
    QString pn;
    QString sn;
    static quint32 overNum = 0;
    pnList.clear();
    while(!ts.atEnd())
    {
        lineNum++;
        strline = ts.readLine().trimmed();
        if(!strline.isEmpty())
        {
            sec = strline.split(" ",QString::SkipEmptyParts);
            if(sec.indexOf(tr("[Batch]")) >= 0)
            {
                if(status != 0)
                {
                    output->append(QString(tr("传感器文件格式错误！应以[Batch]或[Product]开头 第%1行")).arg(lineNum));
                    status = 0;
                }
                status = 1;
                overNum = 0;
            }
            else if(sec.indexOf(tr("[Product]")) >= 0)
            {
                if((status != 0)&&(status != 2))
                {
                    output->append(QString(tr("传感器文件格式错误！缺批次号或传感器数据 第%1行")).arg(lineNum));
                }
                status = 3;
            }
            else if(status == 1)
            {
                batch = sec[0];
                status = 2;
            }
            else if(status == 2)
            {
               output->append(QString(tr("传感器文件格式错误！缺[Product]段头 第%1行")).arg(lineNum));
               status = 0;
            }
            else if(status == 3)
            {
                if(sec.size() >= 2)
                {
                    pn = sec[0];
                    sn = sec[1];
                    status = 4;
                }
                else
                {
                    output->append(QString(tr("传感器文件格式错误！缺产品编号或传感器编号 第%1行")).arg(lineNum));
                    status = 0;
                }
            }
            else if(status == 4)
            {
                if(sec.size()>=2)
                {
                    bool bk,bb;
                    double k,b;
                    k = sec[0].toDouble(&bk);
                    b = sec[1].toDouble(&bb);
                    if(bk & bb)
                    {
                        pn = QString(tr("%1-%2")).arg(batch).arg(pn);
                        if(pnList.indexOf(pn) != -1)
                        {
                            overNum++;
                            output->append(QString(tr("产品编号：%1 重号，第%2行！！")).arg(pn).arg(lineNum));
                            pn = QString(tr("%1_%2")).arg(pn).arg(overNum,2,10,QLatin1Char('0'));
                            output->append(QString(tr("重新编号：%1")).arg(pn));
                        }
                        pnList.append(pn);
                        if((k > 0.011719107638783) && (fabs(b) < 3.0))
                        {
                            product_t pt;
                            pt.pn = pn;
                            pt.sn = sn;
                            pt.k = k;
                            pt.b = b;
                            productList.append(pt);
                            output->append(QString(tr("产品号：%1,传感器号：%2 传感器方程：k=%3，b=%4"))
                                    .arg(pt.pn).arg(pt.sn).arg(pt.k).arg(pt.b));
                        }
                        else
                        {
                            output->append(QString(tr("产品编号：%1 传感器数据超限，第%2行！！")).arg(pn).arg(lineNum));
                        }
                    }
                    else
                    {
                        output->append(QString(tr("产品编号：%1 传感器数据格式错误，第%2行！！")).arg(pn).arg(lineNum));
                    }
                }
                else
                {
                    output->append(QString(tr("传感器文件格式错误！缺传感器方程数据k或b 第%1行")).arg(lineNum));
                }
                status = 0;
            }
            else
            {
                status = 0;
            }
        }
    }
    for(int i = 0;i < productList.size();i++)
    {
        product_t pd = productList.at(i);
        pn = outPath + tr("/") + pd.pn +".bin";
        sn = outPath + tr("/") + pd.pn +".rep";
        qint16 Qk = (qint16)(3.0*128/pd.k + 0.5);
        qint16 Qb = (qint16)(pd.b/3.0*32768 + 0.5);
        strline = QString(tr("_temperature_qk"));
        bool res;
        res = coff.SetSymbolValue(symbol_qk,&Qk);
        res &= coff.SetSymbolValue(symbol_qb,&Qb);
        if(res)
        {
            if(coff.Write(pn))
            {
                QFile frep(sn);
                if(frep.open(QFileDevice::WriteOnly|QFileDevice::Text))
                {
                    QTextStream out(&frep);
                    out<<QString(tr("产品号：%1,传感器号：%2 传感器方程：k=%3，b=%4 Qk=%5 Qb=%6"))
                        .arg(pd.pn).arg(pd.sn).arg(pd.k).arg(pd.b).arg(Qk,4,16,QLatin1Char('0')).arg(Qb,4,16,QLatin1Char('0'));
                    frep.close();
                    output->append(QString(tr("产品编号：%1固件生成成功")).arg(pd.pn));
                }
                else
                {
                    output->append(QString(tr("不能生成报告文件(产品编号%1)")).arg(pd.pn));
                }
            }
            else
            {
                output->append(QString(tr("产品编号：%1固件文件写入错误")).arg(pd.pn));
            }
        }
        else
        {
            output->append(QString(tr("OUT文件不包含Qk或Qb的数据实体，生成固件失败")));
        }
    }
    output->append(tr("固件生成结束"));
    setCursor(cs);
}

/**
 * @brief MainWindow::coffProperty
 */
void MainWindow::coffProperty()
{
    statusBar()->showMessage(tr("填充COFF文件属性视图"), 5000);
    QCursor cs = cursor();
    setCursor(Qt::WaitCursor);
    coffInfo->clear();
    sectionInfo->clear();
    symWidget->clear();
    if(coff.GetStaus())
    {
        TCoffHeader *coffHeader = coff.GetCoffHeader();
        if(coffHeader != NULL)
        {
            QtProperty *groupItem =m_pVarManager->addProperty(QtVariantPropertyManager::groupTypeId(),QStringLiteral("COFF文件头"));
            QtVariantProperty *item =m_pVarManager->addProperty(QVariant::String, QStringLiteral("COFF版本"));
            item->setValue(QString(tr("0x%1")).arg(coffHeader->usVersionID,4,16,QLatin1Char('0')));
            groupItem->addSubProperty(item);

            item =m_pVarManager->addProperty(QVariant::String, QStringLiteral("编译时间戳"));
            item->setValue(QDateTime::fromTime_t(coffHeader->iTimeStamp).toString());
            groupItem->addSubProperty(item);

            item =m_pVarManager->addProperty(QVariant::Int, QStringLiteral("段结构数目"));
            item->setValue(coffHeader->usSectionNumber);
            groupItem->addSubProperty(item);

            item =m_pVarManager->addProperty(QVariant::String, QStringLiteral("符号表地址"));
            item->setValue(QString(tr("0x%1")).arg(coffHeader->uiSymbolPointer,8,16,QLatin1Char('0')));
            groupItem->addSubProperty(item);

            item =m_pVarManager->addProperty(QVariant::Int, QStringLiteral("符号表结构数目"));
            item->setValue(coffHeader->uiSymbolEntryNumber);
            groupItem->addSubProperty(item);

            item =m_pVarManager->addProperty(QVariant::Int, QStringLiteral("可选文件头长度"));
            item->setValue(coffHeader->uiOptionalHeaderBytes);
            groupItem->addSubProperty(item);

            item =m_pVarManager->addProperty(QVariant::String, QStringLiteral("COFF标识"));
            item->setValue(QString(tr("0x%1")).arg(coffHeader->uiFlags,4,16,QLatin1Char('0')));
            groupItem->addSubProperty(item);

            item =m_pVarManager->addProperty(QVariant::String, QStringLiteral("芯片编码"));
            item->setValue(QString(tr("0x%1")).arg(coffHeader->uiTargetID,4,16,QLatin1Char('0')));
            groupItem->addSubProperty(item);

            coffInfo->addProperty(groupItem);
        }
        TCoffOptionalHeader *optHeader = coff.GetOptioHeader();
        if(optHeader != NULL)
        {

            QtProperty *groupItem =m_pVarManager->addProperty(QtVariantPropertyManager::groupTypeId(),QStringLiteral("COFF可选文件头"));
            QtVariantProperty *item =m_pVarManager->addProperty(QVariant::String, QStringLiteral("COFF可选文件头标识"));
            item->setValue(QString(tr("0x%1")).arg(optHeader->usOptionalHeaderID,4,16,QLatin1Char('0')));
            groupItem->addSubProperty(item);

            item =m_pVarManager->addProperty(QVariant::String, QStringLiteral("COFF可选文件头版本"));
            item->setValue(QString(tr("0x%1")).arg(optHeader->usVersionStamp,4,16,QLatin1Char('0')));
            groupItem->addSubProperty(item);

            item =m_pVarManager->addProperty(QVariant::String, QStringLiteral("程序入口"));
            item->setValue(QString(tr("0x%1")).arg(optHeader->uiEntryPoint,8,16,QLatin1Char('0')));
            groupItem->addSubProperty(item);

            item =m_pVarManager->addProperty(QVariant::String, QStringLiteral(".text段偏移地址"));
            item->setValue(QString(tr("0x%1")).arg(optHeader->uiTextAddress,8,16,QLatin1Char('0')));
            groupItem->addSubProperty(item);

            item =m_pVarManager->addProperty(QVariant::Int, QStringLiteral(".text段长度"));
            item->setValue(optHeader->uiTextSize);
            groupItem->addSubProperty(item);

            item =m_pVarManager->addProperty(QVariant::String, QStringLiteral(".data段偏移地址"));
            item->setValue(QString(tr("0x%1")).arg(optHeader->uiDataAddress,8,16,QLatin1Char('0')));
            groupItem->addSubProperty(item);

            item =m_pVarManager->addProperty(QVariant::Int, QStringLiteral(".data段长度"));
            item->setValue(optHeader->uiDataSize);
            groupItem->addSubProperty(item);

            item =m_pVarManager->addProperty(QVariant::Int, QStringLiteral(".bss段长度"));
            item->setValue(optHeader->uiBssSize);
            groupItem->addSubProperty(item);

            coffInfo->addProperty(groupItem);
        }

        TSectionHeader *sectionHeader = coff.GetSectionHeader();
        if(sectionHeader != NULL)
        {
            for(quint16 i = 0;i < coffHeader->usSectionNumber;i++)
            {
                QtProperty *groupItem =m_pVarManager1->addProperty(QtVariantPropertyManager::groupTypeId(),
                                                                 QString(tr("%1段 %2")).arg(coff.GetSectionName(i)).arg(i+1));
                QtVariantProperty *item =m_pVarManager1->addProperty(QVariant::String, QStringLiteral("段实际地址"));
                item->setValue(QString(tr("0x%1")).arg(sectionHeader[i].uiPysicalAddress,8,16,QLatin1Char('0')));
                groupItem->addSubProperty(item);

                item =m_pVarManager1->addProperty(QVariant::String, QStringLiteral("段虚地址"));
                item->setValue(QString(tr("0x%1")).arg(sectionHeader[i].uiVirtalAddress,8,16,QLatin1Char('0')));
                groupItem->addSubProperty(item);

                item =m_pVarManager1->addProperty(QVariant::Int, QStringLiteral("段长度"));
                item->setValue(sectionHeader[i].uiSectionSize);
                groupItem->addSubProperty(item);

                item =m_pVarManager1->addProperty(QVariant::String, QStringLiteral("段文件偏移地址"));
                item->setValue(QString(tr("0x%1")).arg(sectionHeader[i].uiRawDataPointer,8,16,QLatin1Char('0')));
                groupItem->addSubProperty(item);

                item =m_pVarManager1->addProperty(QVariant::String, QStringLiteral("段标识"));
                item->setValue(QString(tr("0x%1")).arg(sectionHeader[i].uiFlags,4,16,QLatin1Char('0')));
                groupItem->addSubProperty(item);

                item =m_pVarManager1->addProperty(QVariant::Int, QStringLiteral("段存贮页号"));
                item->setValue(sectionHeader[i].usMemoryPageNumber);
                groupItem->addSubProperty(item);

                sectionInfo->addProperty(groupItem);
            }
        }

        TSmallSectionHeader *smsectionHeader = coff.GetSmalSectionHeader();
        if(smsectionHeader != NULL)
        {
            for(quint16 i = 0;i < coffHeader->usSectionNumber;i++)
            {
                QtProperty *groupItem =m_pVarManager1->addProperty(QtVariantPropertyManager::groupTypeId(),
                                                             QString(tr("%1段 %2")).arg(coff.GetSectionName(i)).arg(i+1));
                QtVariantProperty *item =m_pVarManager1->addProperty(QVariant::String, QStringLiteral("段实际地址"));
                item->setValue(QString(tr("0x%1")).arg(smsectionHeader[i].uiPysicalAddress,8,16,QLatin1Char('0')));
                groupItem->addSubProperty(item);

                item =m_pVarManager1->addProperty(QVariant::String, QStringLiteral("段虚地址"));
                item->setValue(QString(tr("0x%1")).arg(smsectionHeader[i].uiVirtalAddress,8,16,QLatin1Char('0')));
                groupItem->addSubProperty(item);

                item =m_pVarManager1->addProperty(QVariant::Int, QStringLiteral("段长度"));
                item->setValue(smsectionHeader[i].uiSectionSize);
                groupItem->addSubProperty(item);

                item =m_pVarManager1->addProperty(QVariant::String, QStringLiteral("段文件偏移地址"));
                item->setValue(QString(tr("0x%1")).arg(smsectionHeader[i].uiRawDataPointer,8,16,QLatin1Char('0')));
                groupItem->addSubProperty(item);

                item =m_pVarManager1->addProperty(QVariant::String, QStringLiteral("段标识"));
                item->setValue(QString(tr("0x%1")).arg(smsectionHeader[i].uiFlags,4,16,QLatin1Char('0')));
                groupItem->addSubProperty(item);

                item =m_pVarManager1->addProperty(QVariant::Int, QStringLiteral("段存贮页号"));
                item->setValue(smsectionHeader[i].usMemoryPageNumber);
                groupItem->addSubProperty(item);

                sectionInfo->addProperty(groupItem);
            }
        }
        TSymbolTable *symtab = coff.GetSymbolTable();
        if(symtab != NULL)
        {
            QStringList tableHead;
            tableHead<<QString(tr("符号名"))<<QString(tr("值"))<<QString(tr("段号"))<<QString(tr("存储类"))
                    <<QString(tr("存储扩展"))<<QString(tr("符号类型"));
            symWidget->setHorizontalHeaderLabels(tableHead);
            symWidget->setRowCount(coffHeader->uiSymbolEntryNumber);
            quint32 ind = 0;
            qint8 iMem = 0;
            while(ind < coffHeader->uiSymbolEntryNumber)
            {
                if(iMem == 0)
                {
                    QString strName;
                    strName = (const char *)symtab[ind].sName.sName;
                    if(strName.size() == 0)
                    {
                        char *stList = coff.GetString();
                        strName = (const char*)&stList[symtab[ind].sName.uiIndex[1]];
                    }
                    else if(strName.size() > 8)
                    {
                        strName = strName.left(8);
                    }
                    symWidget->setItem(ind,0,new QTableWidgetItem(strName));
                }
                symWidget->setItem(ind,1,new QTableWidgetItem(QString(tr("0x%1")).arg(symtab[ind].lSymValue,8,16,QLatin1Char('0'))));
                symWidget->setItem(ind,2,new QTableWidgetItem(QString(tr("%1")).arg(symtab[ind].siNum)));
                symWidget->setItem(ind,3,new QTableWidgetItem(QString(tr("0x%1")).arg(symtab[ind].cStorClass,4,16,QLatin1Char('0'))));
                symWidget->setItem(ind,4,new QTableWidgetItem(QString(tr("%1")).arg(symtab[ind].cMEM)));
                symWidget->setItem(ind,5,new QTableWidgetItem(QString(tr("0x%1")).arg(symtab[ind].res,4,16,QLatin1Char('0'))));

                iMem = symtab[ind].cMEM;
                ind++;
            }
        }
    }
    setCursor(cs);
}

