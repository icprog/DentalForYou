#ifndef PREVIEW_H
#define PREVIEW_H

#include <QTextEdit>
#include <QDesktopWidget>
#include <QRect>
#include <QString>

class Preview : public QTextEdit
{
public:
    static Preview *Instance();

    void setHtmlText(const QString &text);

private:
    static Preview *_instance;

    Preview();
    ~Preview();
};

#endif // PREVIEW_H
