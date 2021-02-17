#ifndef DBCLONE_H
#define DBCLONE_H



#include <QString>
#include <QSqlDatabase>

// Use this class to clone a database connection
// for multithreaded purposes


class DBInnerClone
{
private:
    QString m_clonedConnName;
    QSqlDatabase m_db;
public:
    DBInnerClone(const QString &);
    ~DBInnerClone();
    QSqlDatabase db();
    friend class DbClone;
};

class DBClone {
private:
    DBInnerClone *m_innerClone;
public:
    DBClone(const QString &);
    ~DBClone();
    QSqlDatabase operator()();
};

#endif // DBCLONE_H
