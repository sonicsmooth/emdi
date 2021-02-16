#include "dbclone.h"

#include <QDebug>
#include <QThread>
#include <QUuid>

DBInnerClone::DBInnerClone(const QString & connName) :
    m_clonedConnName(QUuid::createUuid().toString()),
    m_db(QSqlDatabase::cloneDatabase(connName, m_clonedConnName))
{
    //qDebug() << m_db.connectionName() << QThread::currentThread();
    if (!m_db.open()) {
        throw std::logic_error("Could not open cloned database");
    }
}


DBInnerClone::~DBInnerClone() {
    m_db.close();
}

QSqlDatabase DBInnerClone::db() {
    return m_db;
}

DBClone::DBClone(const QString & connName) :
    m_innerClone(new DBInnerClone(connName)) {

}

DBClone::~DBClone() {
    QString cloneConnName = m_innerClone->db().connectionName();
    delete m_innerClone;
    QSqlDatabase::removeDatabase(cloneConnName);
}

QSqlDatabase DBClone::operator()() {
    return m_innerClone->db();
}
