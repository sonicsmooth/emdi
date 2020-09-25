#include "qtemdilib.h"

#include <QDebug>

Qtemdilib::Qtemdilib()
{
#if defined(QT_DEBUG)
    qDebug("Hi from lib 456 qt_debug");
#elif defined(QT_NO_DEBUG)
   qDebug("Hi from lib qt_no_debug");
#endif
}
