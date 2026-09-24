#ifndef SCPIPARSER_H
#define SCPIPARSER_H

#include <QString>
#include "PowerSupply.h"

class ScpiParser {
private:
    PowerSupply& power_supply;

    QString recognize(const QString& command);
public:
    ScpiParser(PowerSupply& ps);
    QString parse(const QString& command);

};

#endif // SCPIPARSER_H
