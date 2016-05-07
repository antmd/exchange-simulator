#ifndef EXCHANGESIMULATOR_SERVER_APIUTIL_H
#define EXCHANGESIMULATOR_SERVER_APIUTIL_H


#include "Quote.h"
#include "TradeRecord.h"
#include "SecurityStatus.h"

#include <string>
#include <memory>
#include <cppconn/statement.h>
#include <cppconn/connection.h>

class APIUtil {
public:
    typedef std::shared_ptr<sql::Connection> ConnPtr;
    typedef std::shared_ptr<sql::Statement> StmtPtr;
    typedef std::shared_ptr<sql::ResultSet> ResultSetPtr;


    APIUtil(const ConnPtr &conn_) : conn_(conn_) { }

    bool securityTryLock(const std::string &symbol, bool &result);

    bool securityUnlock(const std::string &symbol);

    bool securityUpdatePrice(const SecurityStatus &security);

    bool securityQuery(const std::string &symbol, SecurityStatus &result);

    bool orderbookPut(const Quote &quote);

    bool orderbookDelete(const int quote_id);

    bool orderbookQueryMatch(const std::string &symbol, ResultSetPtr &result);

    bool orderbookUpdate(const int quote_id, const int quantity);

    bool systemStatusIsRunning(bool &result);

    bool tradeRecordPut(const TradeRecord &record);

private:
    StmtPtr getStmt();

    bool checkConnection();

    bool execute(const std::string &s);

    bool execute(StmtPtr stmt, const std::string &s);

    bool executeQuery(const std::string &s, std::shared_ptr<sql::ResultSet> &results);

    bool executeQuery(StmtPtr stmt, const std::string &s, std::shared_ptr<sql::ResultSet> &results);

    void logError(const sql::SQLException &e, const std::string &query_string);

    ConnPtr conn_;
};

#endif //EXCHANGESIMULATOR_SERVER_APIUTIL_H
