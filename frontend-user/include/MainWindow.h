#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QComboBox>
#include <QLabel>
#include <Qsci/qsciscintilla.h>
#include <Qsci/qscilexerxml.h>
#include <memory>

#include "XMLParser.h"
#include "HTMLParser.h"
#include "DOCXGenerator.h"
#include "SampleGenerator.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onNewFile();
    void onOpenFile();
    void onOpenHtmlFile();
    void onSaveFile();
    void onSaveFileAs();
    void onExportDocx();
    void onValidateXml();
    void onGenerateSample(int index);
    void onAbout();
    void onTextChanged();

private:
    void setupUI();
    void setupMenuBar();
    void setupToolBar();
    void setupEditor();
    void setupStatusBar();
    void updateTitle();
    void setModified(bool modified);
    bool maybeSave();
    
    // UI 组件
    QsciScintilla *m_editor;
    QComboBox *m_sampleCombo;
    QLabel *m_statusLabel;
    QLabel *m_lineColLabel;
    
    // 核心模块
    std::unique_ptr<XMLParser> m_parser;
    std::unique_ptr<HTMLParser> m_htmlParser;
    std::unique_ptr<DOCXGenerator> m_generator;
    std::unique_ptr<SampleGenerator> m_sampleGenerator;
    
    // 状态
    QString m_currentFile;
    bool m_modified;
};

#endif // MAINWINDOW_H
