#include "dbConnector.h"


//------------------------------------------------------
//                    dbSQLITE
//------------------------------------------------------

dbSQLite::dbSQLite(std::string _dbPath)
{
	dbPath = _dbPath;
}

dbSQLite::~dbSQLite()
{
	sqlite3_close(sdb);
}

int dbSQLite::ConnectDB()
{
	int rc = sqlite3_open(dbPath.c_str(), &sdb);

	if (rc)
	{
		this->~dbSQLite();
		return rc;
	}
	return 0;

}

int The_Callback(void *a_param, int argc, char **argv, char **column) {
	for (int i = 0; i < argc; i++)
		printf("%s,\t", argv[i]);
		printf("\n");
	return 0;
}

int dbSQLite::ExecSQL(std::string _SQLQuery)
{
	int erCount;
	char* error = 0; // buffer f5or error
	int rc = 0;
		rc = erCount = sqlite3_exec(sdb, _SQLQuery.c_str(), The_Callback, NULL, &error);

		if (rc != SQLITE_OK)
		{
			listOfErrors.push_back(sqlite3_errmsg(sdb));
			return 1;
		}

	return 0;
}

std::string dbSQLite::getLastError() {
	int rc = listOfErrors.size();

	if (rc > 0) {
		return listOfErrors[rc - 1];
	}
	else {
		return "NULL";
	}

}

tableVector* dbSQLite::GetTableVector(std::string _SQLQuery)
{
	int rows, columns, rowCtr, colCtr; //rows count , columns count , row control, column control 
	rowCtr = 0; // 0 row - names of colums
	colCtr = 0;
	char** results = NULL; // buffer for result
	char* error; // buffer f5or error
	int rc;
	rc = sqlite3_get_table(sdb, _SQLQuery.c_str(), &results, &rows, &columns, &error); //Executing query

	tableVector *res = new tableVector(rows); // 
	
	if (rc != SQLITE_OK) {
		listOfErrors.push_back(error);
		return NULL;
	}

	if (rows < 1) {

		return res;
	}
	else if (rows > 0)
	{
		while (rowCtr + 1 <= rows)
		{
			(*res)[rowCtr].resize(columns);
			while (colCtr < columns)
			{

				int celPos = ((rowCtr + 1) * columns) + colCtr;
				if (results[celPos] != NULL) {  
					(*res)[rowCtr][colCtr] = results[celPos];
					++colCtr;
				}
				else {
					(*res)[rowCtr][colCtr] = "NULL"; // If cell == NULL
					++colCtr;
				}

			}
			colCtr = 0;
			++rowCtr;
		}
		return res;
	}
	delete[] &results;
	delete error;
	return res;
}

//------------------------------------------------------
//                    dbMSSQL 
//------------------------------------------------------

dbMSSQL::dbMSSQL(const std::string _dbUser, const std::string _dbPass, const std::string _dbAddr, const std::string _dbName) {

	pConn = NULL;
	CoInitialize(NULL);
	
	strConn = "Provider=sqloledb;Data Source=";
	strConn += _dbAddr;
	strConn += ";Initial Catalog=";
	strConn += _dbName;
	dbUser = _dbUser;
	dbPassword = _dbPass;

}


dbMSSQL::~dbMSSQL() {

}

int dbMSSQL::ConnectDB() {

	try {
		pConn.CreateInstance(__uuidof(Connection));
		pConn->ConnectionTimeout = DB_CONNECTION_TIMEOUT;
		pConn->Open(strConn.c_str(), dbUser.c_str(), dbPassword.c_str(), adConnectUnspecified);
	}
	catch (_com_error &err) {
		listOfErrors.push_back(err.ErrorMessage());
		return(1);
	}

	pErr = NULL;

	if ((pConn->Errors->Count) > 0)
	{
		long nCount = pConn->Errors->Count;
		// Collection ranges from 0 to nCount -1.
		for (long i = 0; i < nCount; i++)
		{
			pErr = pConn->Errors->GetItem(i);
			//printf(" Error number: %x %s", pErr->Number,
			listOfErrors.push_back((char*)pErr->Description);
		}
	}
	return 0;

}

tableVector* dbMSSQL::GetTableVector(const std::string _sqlQuery) {
	_RecordsetPtr rec = NULL;
	tableVector* QueryRes = new tableVector(0);
	rec.CreateInstance(__uuidof(Recordset));
	rec->CursorLocation = adUseClient;
	try
	{
		rec->Open(_sqlQuery.c_str(), _variant_t((IDispatch*)pConn, true), adOpenStatic, adLockReadOnly, adCmdText);
	}
	catch (_com_error &err) {
		listOfErrors.push_back(err.ErrorMessage());
		return QueryRes;
	}
	FieldsPtr pFields;
	FieldPtr pField;
	pFields = rec->Fields;
	int columnsCount = pFields->Count;
	int rowsCount = rec->GetRecordCount();

	if (rowsCount == 0) {

		return QueryRes;
	}

	QueryRes->resize(columnsCount);

	_bstr_t valField2;
	int j = 0;
	int i = 0;

	while (i < columnsCount) {
		rec->MoveFirst();
		//Loop through the Record set
		_bstr_t valField1;

		while (!rec->EndOfFile)
		{
			try {
				(*QueryRes)[i].resize(rowsCount);

				if (rec->Fields->GetItem(long(i))->ActualSize != 0)
				{
					valField1 = rec->Fields->GetItem(long(i))->Value;
					(*QueryRes)[i][j] = valField1;
				}
				else
				{
					(*QueryRes)[i][j] = "NULL";
				}
			}
			catch (_com_error &ce)
			{
				listOfErrors.push_back(ce.ErrorMessage());
			}

			rec->MoveNext();

			++j;
		}
		j = 0;
		++i;
	}
	return QueryRes;
}

int dbMSSQL::ExecSQL(std::string _sqlText) {
	//release the relealesation
	return 1;
}

std::string dbMSSQL::getLastError() {
	int rc = listOfErrors.size();

	if (rc > 0) {
		return listOfErrors[rc - 1];
	}
	else {
		return "NULL";
	}

}