#ifndef HSTORCOMMAND_H
#define HSTORCOMMAND_H

#include <QString>
#include <QStringList>


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief The HSTorCommand class
///
/// This is used to build tor command to send it to tor control port
///////////////////////////////////////////////////////////////////////////////////////////////////
class HSTorCommand
{
private:
    QString m_keyword;
    QStringList m_data;
    QStringList m_arguments;

public:
    explicit HSTorCommand();
    explicit HSTorCommand(const QString &k);
    explicit HSTorCommand(const QString &k, const QString &arg);
    explicit HSTorCommand(const QString &k, const QStringList &args);

    virtual ~HSTorCommand();

    void setKeyword(const QString &k);
    QString keyword() const;

    void setArguments(const QStringList &args);
    QStringList arguments() const;

    void addArgumens(const QStringList &args);
    void addArgument(const QString &arg);

    void addData(const QString &d);

    void reset();
    void clear();

    QString escape(const QString &unescaped) const;
    QString toString() const;

};

#endif // HSTORCOMMAND_H
