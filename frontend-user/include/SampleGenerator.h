#ifndef SAMPLEGENERATOR_H
#define SAMPLEGENERATOR_H

#include <QString>
#include <QStringList>

class SampleGenerator {
public:
    enum SampleType {
        BasicText = 0,
        RichText,
        TableSample,
        ImageSample,
        CompleteSample
    };
    
    SampleGenerator();
    
    // 生成指定类型的示例 XML
    QString generate(SampleType type);
    
    // 获取所有示例类型名称
    QStringList sampleNames() const;
    
    // 获取示例数量
    int sampleCount() const { return 5; }

private:
    QString generateBasicText();
    QString generateRichText();
    QString generateTableSample();
    QString generateImageSample();
    QString generateCompleteSample();
};

#endif // SAMPLEGENERATOR_H
