#pragma once

#include <vector>
#include <list>
#include <fstream>
#include "windows.h"
#include <sqltypes.h> 
#include <sql.h> 
#include <sqlext.h>
#include <cstdlib>
#include <locale>
#include <codecvt>
#include "sqlite3.h"

using namespace std;
typedef std::vector<std::vector<std::string>> tableVector;
typedef std::list<std::list<std::string>> resultList;
 
#pragma comment(lib, "sqlite3.lib")


#import "C:\Program Files\Common Files\System\ADO\msado15.dll" rename("EOF", "EndOfFile")
using namespace ADODB;
constexpr int DB_CONNECTION_TIMEOUT = 3;



class connector {
		public:
		virtual int ConnectDB() = 0;
		virtual int ExecSQL(string _sqlText) = 0;
		virtual tableVector* GetTableVector(string _sqlText) = 0;
		virtual std::string getLastError() = 0;
		virtual ~connector() {};
		//virtual resultList* GetTableList(string _sqlText) = 0;
};


class dbSQLite : connector
{
public:
	dbSQLite(std::string _dbPath);
	int ConnectDB() override;
	int ExecSQL(string _sqlText) override;
	tableVector* GetTableVector(string _sqlText) override;
	std::string getLastError() override;
	~dbSQLite() override;
private:
	std::vector<std::string> listOfErrors;
	std::string dbPath;
	sqlite3* sdb;
};




class dbMSSQL : connector
{
public:
	std:: string _connString;
	dbMSSQL(const std::string _dbUser, const std::string _dbPass, const std::string _dbAddr, const std::string _dbName) ;
	int ConnectDB() override;
	int ExecSQL(std::string _sqlText) override;
	tableVector* GetTableVector(std::string _sqlText) override;
	std::string getLastError() override;
	~dbMSSQL() override;
private:
	int createConnection() {};
	std::vector<std::string> listOfErrors;
	std::string strConn;
	std::string dbUser;
	std::string dbPassword;
	_ConnectionPtr pConn;
	ErrorPtr pErr;
};



