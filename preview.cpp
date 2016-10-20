#include "preview.h"

Preview *Preview::Instance()
{
    if (_instance == 0)
    {
        _instance = new Preview();
    }

    return _instance;
}

void Preview::setHtmlText(const QString &text)
{
    this->setHtml(text);
}

Preview::Preview()
{
    QDesktopWidget desktop;
    QRect rect = desktop.availableGeometry(desktop.primaryScreen());
    rect.setX(rect.center().x() - (this->width()/2));
    rect.setY(rect.center().y() - (this->height()/2));
    rect.setHeight(400);
    rect.setWidth(500);
    this->setGeometry(rect);
}

Preview::~Preview()
{

}

Preview *Preview::_instance = 0;
