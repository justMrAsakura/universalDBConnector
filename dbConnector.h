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


typedef std::vector<std::vector<std::string>> tableVector;
typedef std::list<std::list<std::string>> resultList;
 
using namespace std;

// komment this if you want use only one type DB.
#define _SQLite 
#define _MSSQL

#ifdef _SQLite
	#pragma comment(lib, "sqlite3.lib")
#endif

#ifdef _MSSQL
	#import "C:\Program Files\Common Files\System\ADO\msado15.dll" rename("EOF", "EndOfFile")
	using namespace ADODB;
	constexpr int DB_CONNECTION_TIMEOUT = 3;

#endif 

#ifdef _SQLite
class dbSQLite
{
public:
	dbSQLite(std::string _dbPath);
	int connectDb();
	int execSQL(std::string _SQLQuery);
	std::string getLastError();
	tableVector* getTableVector(std::string _SQLQuery);
	
	~dbSQLite();
private:
	std::vector<std::string> listOfErrors;
	std::string dbPath;
	sqlite3* sdb;
	
};
#endif

#ifdef _MSSQL

class dbMSSQL
{
public:
	std:: string _connString;
	dbMSSQL() ;
	int connectDb(const std::string _dbUser, const std::string _dbPass, const std::string _dbAddr, const std::string _dbName);
	int execSQL(std::string _SQLQuery);
	std::string getLastError();
	tableVector* getTableVector(const std::string _SQLQuery);
	~dbMSSQL();
private:
	std::vector<std::string> listOfErrors;
	_ConnectionPtr pConn;
	ErrorPtr pErr;

};


#endif 
