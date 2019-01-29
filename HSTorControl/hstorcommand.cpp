#include "hstorcommand.h"

HSTorCommand::HSTorCommand()
{

}

HSTorCommand::HSTorCommand(const QString &k, const QString &arg)
{
    this->m_keyword = k;
    this->m_arguments<<arg;
}

HSTorCommand::HSTorCommand(const QString &k, const QStringList &args)
{
    this->m_keyword = k;
    this->m_arguments = args;
}

void HSTorCommand::setKeyword(const QString &k)
{
    this->m_keyword = k;
}

QString HSTorCommand::keyword() const
{
    return this->m_keyword;
}

void HSTorCommand::setArguments(const QStringList &args)
{
    this->m_arguments = args;
}

QStringList HSTorCommand::arguments() const
{
    return this->m_arguments;
}

void HSTorCommand::addArgumens(const QStringList &args)
{
    for(const QString &arg:args){
        this->m_arguments<<arg;
    }
}

void HSTorCommand::addArgument(const QString &arg)
{
    this->m_arguments<<arg;
}

void HSTorCommand::addData(const QString &d)
{
    this->m_data<<d;
}

void HSTorCommand::reset()
{
    this->m_keyword.clear();
    this->m_arguments.clear();
    this->m_data.clear();
}

void HSTorCommand::clear()
{
    this->m_keyword.clear();
    this->m_arguments.clear();
    this->m_data.clear();
}

/** Escapes any special characters in this command. */
QString HSTorCommand::escape(const QString &unescaped) const
{
    QString str = unescaped;
    if (str.startsWith(".")) {
      str.prepend(".");
    }
    if (str.endsWith("\r")) {
      str.append("\n");
    } else {
      str.append("\r\n");
    }
    return str;
}


/** Formats a command according to Tor's Control Protocol V1. The proper
 * format of a command is as follows:
 *
 *   Command = Keyword Arguments CRLF / "+" Keyword Arguments CRLF Data
 *   Keyword = 1*ALPHA
 *   Arguments = *(SP / VCHAR)
 */
QString HSTorCommand::toString() const
{
    int i;
    QString str;

    /* If this command contains data, then a "+" is prepended to the keyword */
    if (this->m_data.size() > 0) {
      str = "+";
    }
    str += this->m_keyword + " ";

    /* Append all specified arguments separated by a space */
    str += this->m_arguments.join(" ");

    /* Append whatever data lines have been specified */
    if (this->m_data.size() > 0) {
      str += "\r\n";
      for (i = 0; i < this->m_data.size(); i++) {
        str += escape(this->m_data.at(i));
      }
      str += ".";
    }
    return str.append("\r\n");
}

HSTorCommand::~HSTorCommand()
{

}
