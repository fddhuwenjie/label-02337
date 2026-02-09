#include "MainWindow.h"
#include "Logger.h"
#include <QApplication>
#include <QFileDialog>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QAction>
#include <QMenu>
#include <QFont>
#include <QTextStream>
#include <QFile>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_editor(nullptr)
    , m_sampleCombo(nullptr)
    , m_statusLabel(nullptr)
    , m_lineColLabel(nullptr)
    , m_parser(std::make_unique<XMLParser>())
    , m_htmlParser(std::make_unique<HTMLParser>())
    , m_generator(std::make_unique<DOCXGenerator>())
    , m_sampleGenerator(std::make_unique<SampleGenerator>())
    , m_modified(false)
{
    LOG_INFO("应用程序启动");
    
    setupUI();
    setupMenuBar();
    setupToolBar();
    setupStatusBar();
    updateTitle();
    
    resize(1200, 800);
}

MainWindow::~MainWindow() {}

void MainWindow::setupUI() {
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    
    setupEditor();
    layout->addWidget(m_editor);
    
    setCentralWidget(centralWidget);
}

void MainWindow::setupEditor() {
    m_editor = new QsciScintilla(this);
    
    // 设置 XML 语法高亮
    QsciLexerXML *lexer = new QsciLexerXML(m_editor);
    lexer->setDefaultFont(QFont("Consolas", 11));
    m_editor->setLexer(lexer);
    
    // 设置编辑器属性
    m_editor->setUtf8(true);
    m_editor->setMarginType(0, QsciScintilla::NumberMargin);
    m_editor->setMarginWidth(0, "00000");
    m_editor->setMarginLineNumbers(0, true);
    
    // 设置自动缩进
    m_editor->setAutoIndent(true);
    m_editor->setIndentationWidth(4);
    m_editor->setIndentationsUseTabs(false);
    
    // 设置代码折叠
    m_editor->setFolding(QsciScintilla::BoxedTreeFoldStyle);
    
    // 设置当前行高亮
    m_editor->setCaretLineVisible(true);
    m_editor->setCaretLineBackgroundColor(QColor("#f0f0f0"));
    
    // 设置括号匹配
    m_editor->setBraceMatching(QsciScintilla::SloppyBraceMatch);
    
    // 连接信号
    connect(m_editor, &QsciScintilla::textChanged, this, &MainWindow::onTextChanged);
    connect(m_editor, &QsciScintilla::cursorPositionChanged, this, [this](int line, int col) {
        m_lineColLabel->setText(QString("行: %1  列: %2").arg(line + 1).arg(col + 1));
    });
}

void MainWindow::setupMenuBar() {
    QMenuBar *menuBar = this->menuBar();
    
    // 文件菜单
    QMenu *fileMenu = menuBar->addMenu("文件(&F)");
    
    QAction *newAction = fileMenu->addAction("新建(&N)");
    newAction->setShortcut(QKeySequence::New);
    connect(newAction, &QAction::triggered, this, &MainWindow::onNewFile);
    
    QAction *openAction = fileMenu->addAction("打开(&O)...");
    openAction->setShortcut(QKeySequence::Open);
    connect(openAction, &QAction::triggered, this, &MainWindow::onOpenFile);
    
    QAction *openHtmlAction = fileMenu->addAction("导入 HTML(&I)...");
    openHtmlAction->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_O));
    connect(openHtmlAction, &QAction::triggered, this, &MainWindow::onOpenHtmlFile);
    
    QAction *saveAction = fileMenu->addAction("保存(&S)");
    saveAction->setShortcut(QKeySequence::Save);
    connect(saveAction, &QAction::triggered, this, &MainWindow::onSaveFile);
    
    QAction *saveAsAction = fileMenu->addAction("另存为(&A)...");
    saveAsAction->setShortcut(QKeySequence::SaveAs);
    connect(saveAsAction, &QAction::triggered, this, &MainWindow::onSaveFileAs);
    
    fileMenu->addSeparator();
    
    QAction *exportAction = fileMenu->addAction("导出 DOCX(&E)...");
    exportAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_E));
    connect(exportAction, &QAction::triggered, this, &MainWindow::onExportDocx);
    
    fileMenu->addSeparator();
    
    QAction *exitAction = fileMenu->addAction("退出(&X)");
    exitAction->setShortcut(QKeySequence::Quit);
    connect(exitAction, &QAction::triggered, this, &QMainWindow::close);
    
    // 编辑菜单
    QMenu *editMenu = menuBar->addMenu("编辑(&E)");
    
    QAction *undoAction = editMenu->addAction("撤销(&U)");
    undoAction->setShortcut(QKeySequence::Undo);
    connect(undoAction, &QAction::triggered, m_editor, &QsciScintilla::undo);
    
    QAction *redoAction = editMenu->addAction("重做(&R)");
    redoAction->setShortcut(QKeySequence::Redo);
    connect(redoAction, &QAction::triggered, m_editor, &QsciScintilla::redo);
    
    editMenu->addSeparator();
    
    QAction *cutAction = editMenu->addAction("剪切(&T)");
    cutAction->setShortcut(QKeySequence::Cut);
    connect(cutAction, &QAction::triggered, m_editor, &QsciScintilla::cut);
    
    QAction *copyAction = editMenu->addAction("复制(&C)");
    copyAction->setShortcut(QKeySequence::Copy);
    connect(copyAction, &QAction::triggered, m_editor, &QsciScintilla::copy);
    
    QAction *pasteAction = editMenu->addAction("粘贴(&P)");
    pasteAction->setShortcut(QKeySequence::Paste);
    connect(pasteAction, &QAction::triggered, m_editor, &QsciScintilla::paste);
    
    editMenu->addSeparator();
    
    QAction *selectAllAction = editMenu->addAction("全选(&A)");
    selectAllAction->setShortcut(QKeySequence::SelectAll);
    connect(selectAllAction, &QAction::triggered, m_editor, &QsciScintilla::selectAll);
    
    // 工具菜单
    QMenu *toolsMenu = menuBar->addMenu("工具(&T)");
    
    QAction *validateAction = toolsMenu->addAction("验证 XML(&V)");
    validateAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_T));
    connect(validateAction, &QAction::triggered, this, &MainWindow::onValidateXml);
    
    // 帮助菜单
    QMenu *helpMenu = menuBar->addMenu("帮助(&H)");
    
    QAction *aboutAction = helpMenu->addAction("关于(&A)...");
    connect(aboutAction, &QAction::triggered, this, &MainWindow::onAbout);
}


void MainWindow::setupToolBar() {
    QToolBar *toolBar = addToolBar("主工具栏");
    toolBar->setMovable(false);
    toolBar->setIconSize(QSize(24, 24));
    
    // 新建按钮
    QAction *newAction = toolBar->addAction("📄 新建");
    connect(newAction, &QAction::triggered, this, &MainWindow::onNewFile);
    
    // 打开按钮
    QAction *openAction = toolBar->addAction("📂 打开");
    connect(openAction, &QAction::triggered, this, &MainWindow::onOpenFile);
    
    // 保存按钮
    QAction *saveAction = toolBar->addAction("💾 保存");
    connect(saveAction, &QAction::triggered, this, &MainWindow::onSaveFile);
    
    toolBar->addSeparator();
    
    // 示例选择下拉框
    QLabel *sampleLabel = new QLabel(" 示例模板: ", toolBar);
    toolBar->addWidget(sampleLabel);
    
    m_sampleCombo = new QComboBox(toolBar);
    m_sampleCombo->addItem("-- 选择示例 --");
    QStringList sampleNames = m_sampleGenerator->sampleNames();
    for (const QString &name : sampleNames) {
        m_sampleCombo->addItem(name);
    }
    m_sampleCombo->setMinimumWidth(150);
    connect(m_sampleCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onGenerateSample);
    toolBar->addWidget(m_sampleCombo);
    
    toolBar->addSeparator();
    
    // 验证按钮
    QAction *validateAction = toolBar->addAction("✓ 验证 XML");
    connect(validateAction, &QAction::triggered, this, &MainWindow::onValidateXml);
    
    // 导出按钮
    QAction *exportAction = toolBar->addAction("📥 导出 DOCX");
    connect(exportAction, &QAction::triggered, this, &MainWindow::onExportDocx);
}

void MainWindow::setupStatusBar() {
    QStatusBar *status = statusBar();
    
    m_statusLabel = new QLabel("就绪", this);
    status->addWidget(m_statusLabel, 1);
    
    m_lineColLabel = new QLabel("行: 1  列: 1", this);
    status->addPermanentWidget(m_lineColLabel);
}

void MainWindow::updateTitle() {
    QString title = "XML to DOCX Converter";
    if (!m_currentFile.isEmpty()) {
        QFileInfo info(m_currentFile);
        title = info.fileName() + " - " + title;
    } else {
        title = "未命名 - " + title;
    }
    if (m_modified) {
        title = "* " + title;
    }
    setWindowTitle(title);
}

void MainWindow::setModified(bool modified) {
    m_modified = modified;
    updateTitle();
}

void MainWindow::onTextChanged() {
    setModified(true);
}

bool MainWindow::maybeSave() {
    if (!m_modified) {
        return true;
    }
    
    QMessageBox::StandardButton ret = QMessageBox::warning(
        this, "保存更改",
        "文档已被修改。\n是否保存更改？",
        QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel
    );
    
    if (ret == QMessageBox::Save) {
        onSaveFile();
        return !m_modified;
    } else if (ret == QMessageBox::Cancel) {
        return false;
    }
    return true;
}

void MainWindow::onNewFile() {
    if (!maybeSave()) return;
    
    m_editor->clear();
    m_currentFile.clear();
    setModified(false);
    m_statusLabel->setText("新建文件");
}

void MainWindow::onOpenFile() {
    if (!maybeSave()) return;
    
    QString fileName = QFileDialog::getOpenFileName(
        this, "打开 XML 文件", QString(),
        "XML 文件 (*.xml);;所有文件 (*.*)"
    );
    
    if (fileName.isEmpty()) return;
    
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "错误", "无法打开文件: " + file.errorString());
        return;
    }
    
    QTextStream in(&file);
    in.setEncoding(QStringConverter::Utf8);
    m_editor->setText(in.readAll());
    file.close();
    
    m_currentFile = fileName;
    setModified(false);
    m_statusLabel->setText("已打开: " + fileName);
}

void MainWindow::onSaveFile() {
    if (m_currentFile.isEmpty()) {
        onSaveFileAs();
        return;
    }
    
    QFile file(m_currentFile);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "错误", "无法保存文件: " + file.errorString());
        return;
    }
    
    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);
    out << m_editor->text();
    file.close();
    
    setModified(false);
    m_statusLabel->setText("已保存: " + m_currentFile);
}

void MainWindow::onSaveFileAs() {
    QString fileName = QFileDialog::getSaveFileName(
        this, "保存 XML 文件", QString(),
        "XML 文件 (*.xml);;所有文件 (*.*)"
    );
    
    if (fileName.isEmpty()) return;
    
    m_currentFile = fileName;
    onSaveFile();
}

void MainWindow::onExportDocx() {
    QString xmlContent = m_editor->text();
    
    if (xmlContent.trimmed().isEmpty()) {
        QMessageBox::warning(this, "错误", "请先输入 XML 内容");
        return;
    }
    
    // 验证 XML
    QString errorMsg;
    if (!m_parser->validate(xmlContent, errorMsg)) {
        QMessageBox::warning(this, "XML 格式错误", errorMsg);
        return;
    }
    
    // 选择保存路径
    QString fileName = QFileDialog::getSaveFileName(
        this, "导出 DOCX 文件", "document.docx",
        "Word 文档 (*.docx)"
    );
    
    if (fileName.isEmpty()) return;
    
    // 解析 XML
    auto model = m_parser->parse(xmlContent);
    if (!model) {
        QMessageBox::warning(this, "解析错误", m_parser->lastError());
        return;
    }
    
    // 生成 DOCX
    m_statusLabel->setText("正在生成 DOCX...");
    QApplication::processEvents();
    
    if (m_generator->generate(*model, fileName)) {
        m_statusLabel->setText("导出成功: " + fileName);
        QMessageBox::information(this, "导出成功", 
            QString("DOCX 文件已成功导出到:\n%1").arg(fileName));
    } else {
        m_statusLabel->setText("导出失败");
        QMessageBox::warning(this, "导出失败", m_generator->lastError());
    }
}

void MainWindow::onValidateXml() {
    QString xmlContent = m_editor->text();
    
    if (xmlContent.trimmed().isEmpty()) {
        QMessageBox::warning(this, "提示", "请先输入 XML 内容");
        return;
    }
    
    QString errorMsg;
    if (m_parser->validate(xmlContent, errorMsg)) {
        m_statusLabel->setText("XML 验证通过");
        QMessageBox::information(this, "验证结果", "✓ XML 格式正确");
    } else {
        m_statusLabel->setText("XML 验证失败");
        QMessageBox::warning(this, "验证结果", "✗ " + errorMsg);
    }
}

void MainWindow::onGenerateSample(int index) {
    if (index <= 0) return;
    
    if (!maybeSave()) {
        m_sampleCombo->setCurrentIndex(0);
        return;
    }
    
    SampleGenerator::SampleType type = static_cast<SampleGenerator::SampleType>(index - 1);
    QString sample = m_sampleGenerator->generate(type);
    
    m_editor->setText(sample);
    m_currentFile.clear();
    setModified(false);
    m_statusLabel->setText("已加载示例: " + m_sampleCombo->currentText());
    
    // 重置下拉框
    m_sampleCombo->blockSignals(true);
    m_sampleCombo->setCurrentIndex(0);
    m_sampleCombo->blockSignals(false);
}

void MainWindow::onAbout() {
    QMessageBox::about(this, "关于 XML to DOCX Converter",
        "<h3>XML to DOCX Converter</h3>"
        "<p>版本 1.0.0</p>"
        "<p>基于 Qt/C++ 开发的 XML 配置文件到 DOCX 文档转换工具。</p>"
        "<p><b>功能特性:</b></p>"
        "<ul>"
        "<li>支持文字颜色、字体、对齐设置</li>"
        "<li>支持图片插入及对齐</li>"
        "<li>支持表格行列数、宽度、对齐设置</li>"
        "<li>支持导入 HTML 文件并转换为 XML</li>"
        "<li>使用 QScintilla 实现 XML 语法高亮</li>"
        "<li>导出为标准 DOCX 格式</li>"
        "</ul>"
    );
}

void MainWindow::onOpenHtmlFile() {
    if (!maybeSave()) return;
    
    QString fileName = QFileDialog::getOpenFileName(
        this, "导入 HTML 文件", QString(),
        "HTML 文件 (*.html *.htm);;所有文件 (*.*)"
    );
    
    if (fileName.isEmpty()) return;
    
    LOG_INFO(QString("导入 HTML 文件: %1").arg(fileName));
    
    // 解析 HTML 并转换为 XML
    QString xmlContent = m_htmlParser->convertToXml(
        [&]() {
            QFile file(fileName);
            if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                return QString();
            }
            QTextStream in(&file);
            in.setEncoding(QStringConverter::Utf8);
            return in.readAll();
        }()
    );
    
    if (xmlContent.isEmpty()) {
        QMessageBox::warning(this, "错误", 
            "无法解析 HTML 文件:\n" + m_htmlParser->lastError());
        LOG_ERROR(QString("HTML 解析失败: %1").arg(m_htmlParser->lastError()));
        return;
    }
    
    m_editor->setText(xmlContent);
    m_currentFile.clear();
    setModified(true);
    m_statusLabel->setText("已导入 HTML: " + fileName);
    LOG_INFO("HTML 导入成功");
}
