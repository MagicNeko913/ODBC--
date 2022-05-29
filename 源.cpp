#include<iostream>
#include <tchar.h>
#include <sstream>
#include<string>
#include<windows.h>
#include<sql.h>
#include<sqlext.h>
#include<sqltypes.h>
#define cname_length 10
#define cplace_length 50
#define corder_length 30
#define oname_length 50
#define pname_length 20
using namespace std;
SQLRETURN ret;
SQLHENV henv;
SQLHDBC hdbc;
SQLHSTMT hstmt;
typedef struct {
	SQLCHAR cname[cname_length],
		cplace[cplace_length],
		corder[corder_length],
		oname[oname_length],
		pname[pname_length];
	SQLSMALLINT acode,cccode, money, pprice;
	SQLINTEGER anum, ctel, onum,quantity;
}ORDERING;
ORDERING ordering;

//字符转为宽字符
wchar_t* trstring2wchar(const  char* str)
{
	int mystringsize = (int)(strlen(str) + 1);
	WCHAR* wchart = new wchar_t[mystringsize];
	MultiByteToWideChar(CP_ACP, 0, str, -1, wchart, mystringsize);
	return wchart;

}

//宽字符转换为字符串
string wchar2strstring(string& szDst, WCHAR* wchart)
{
	wchar_t* wtext = wchart;
	DWORD dwNmu = WideCharToMultiByte(CP_OEMCP, NULL, wtext, -1, NULL, 0, NULL, FALSE);
	char* psTest;
	psTest = new char[dwNmu];
	WideCharToMultiByte(CP_OEMCP, NULL, wtext, -1, psTest, dwNmu, NULL, FALSE);
	szDst = psTest;
	delete[]psTest;
	return szDst;
}

//
string wChar2String(LPCWSTR pwszSrc) {
	int nLen = WideCharToMultiByte(CP_ACP, 0, pwszSrc, -1, NULL, 0, NULL, NULL);
	if (nLen <= 0)
		return string("");
	char* pszDst = new char[nLen]; 
	if (NULL == pszDst)
		return string("");
	WideCharToMultiByte(CP_ACP, 0, pwszSrc, -1, pszDst, nLen, NULL, NULL); 
	pszDst[nLen - 1] = 0;
	string strTemp(pszDst);
	delete[] pszDst;
	return strTemp;
}

//字符转换为wstring
wstring CharToWchar(const char* c, size_t m_encode = CP_ACP)
{
	std::wstring str;
	int len = MultiByteToWideChar(m_encode, 0, c, strlen(c), NULL, 0);
	wchar_t* m_wchar = new wchar_t[len + 1];
	MultiByteToWideChar(m_encode, 0, c, strlen(c), m_wchar, len);
	m_wchar[len] = '\0';
	str = m_wchar;
	delete m_wchar;
	return str;
}

//宽字符转字符指针
char* wideCharToMultiByte(wchar_t* pWCStrKey)
{
	//第一次调用确认转换后单字节字符串的长度，用于开辟空间
	int pSize = WideCharToMultiByte(CP_OEMCP, 0, pWCStrKey, wcslen(pWCStrKey), NULL, 0, NULL, NULL);
	char* pCStrKey = new char[pSize + 1];
	//第二次调用将双字节字符串转换成单字节字符串
	WideCharToMultiByte(CP_OEMCP, 0, pWCStrKey, wcslen(pWCStrKey), pCStrKey, pSize, NULL, NULL);
	pCStrKey[pSize] = '\0';
	return pCStrKey;
}

//错误处理
void handleResult(SQLHANDLE hHandle, SQLSMALLINT hType, RETCODE  RetCode)
{
	SQLSMALLINT iRec = 0;
	SQLINTEGER iError;
	WCHAR wszMessage[1000];
	WCHAR wszState[SQL_SQLSTATE_SIZE + 1];
	//处理无效
	if (RetCode == SQL_INVALID_HANDLE)
	{
		fwprintf(stderr, L"Invalid handle!\n");
		return;
	}
	while (SQLGetDiagRec(hType, hHandle, ++iRec, wszState, &iError, wszMessage, (SQLSMALLINT)(sizeof(wszMessage) / sizeof(WCHAR)),
		(SQLSMALLINT*)NULL) == SQL_SUCCESS) {
		// Hide data truncated.. 
		if (wcsncmp(wszState, L"01004", 5))
		{
			fwprintf(stderr, L"[%5.5s]---%s---(%d)\n", wszState, wszMessage, iError);
		}
	}
}

//输出错误
void error(SQLRETURN err) {
	cout<<"相关信息输出： "<<endl;
	switch (err) {
	case	SQL_SUCCESS:puts("****SQL_SUCCESS*****"); break;
	case	SQL_SUCCESS_WITH_INFO:puts("SQL_SUCCESS_WITH_INFO"); break;
	case	SQL_ERROR:puts("SQL_ERROR"); break;
	case	SQL_INVALID_HANDLE:puts("SQL_INVALID_HANDLE"); break;
	case	SQL_NO_DATA_FOUND:puts("SQL_NO_DATA_FOUND"); break;
	case	SQL_NEED_DATA:puts("SQL_NEED_DATA"); break;
	default:puts("err");
	}
}

//连接
void Link()
{
	ret = SQLAllocHandle(SQL_HANDLE_ENV, NULL, &henv);//申请环境句柄
	ret = SQLSetEnvAttr(henv,SQL_ATTR_ODBC_VERSION,(SQLPOINTER)SQL_OV_ODBC3,SQL_IS_INTEGER);
	ret = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);//申请数据库连接句柄

	ret = SQLConnect(hdbc, (SQLWCHAR*)L"TEST", SQL_NTS, (SQLWCHAR*)L"XYL", SQL_NTS, (SQLWCHAR*)L"xuyule010913@", SQL_NTS);

	if (!(ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO)) {
		cout << "Failed to connect to database!" << endl << endl;
	}
	else {
		cout << "Database connection succeeded!" << endl << endl;
	}
}

//释放空间
void free()
{
	SQLFreeHandle(SQL_HANDLE_STMT, hstmt);//释放语句
	SQLFreeHandle(SQL_HANDLE_DBC, hdbc);//释放连接
	SQLFreeHandle(SQL_HANDLE_ENV, henv);//释放环境
}

//断开连接
void Unlink() {
	SQLDisconnect(hdbc);
	SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
	SQLFreeHandle(SQL_HANDLE_ENV, henv);
}


//查询
void Action_Admin();
void Action_Customer();

void Inquire_Admin() {
	ret = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
	SQLWCHAR sql1[] = L"SELECT * FROM admin";
	ret = SQLExecDirect(hstmt, sql1, SQL_NTS);
	cout << "AdminNumber" << "  " << "Password" << endl << endl;
	if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO)
	{
		SQLCHAR str1[50], str2[50];
		SQLLEN len_str1, len_str2;
		/*SQLFetch函数的功能是将结果集的当前记录指针移至下一个记录；*/
		while (SQLFetch(hstmt) != SQL_NO_DATA)
		{
			/*SQLGetData函数的功能是提取结果集中当前记录的某个字段值*/
			SQLGetData(hstmt, 1, SQL_C_CHAR, str1, sizeof(str1), &len_str1);   //获取第一列数据
			SQLGetData(hstmt, 2, SQL_C_CHAR, str2, sizeof(str2), &len_str2);
			cout << str1 << "  " << str2 << endl;
		}
	}
	else {
		cout << "Query failed!" << endl;
		handleResult(hstmt, SQL_HANDLE_STMT, ret);
		error(ret);
	}
	ret = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
	free();
	cout << endl << endl;
	Action_Admin();
}

void Inquire_Customer() {
	ret = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
	SQLWCHAR sql1[] = L"SELECT * FROM customer";
	ret = SQLExecDirect(hstmt, sql1, SQL_NTS);

	if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO)
	{
		SQLCHAR str1[50], str2[50], str3[50], str4[50];
		SQLLEN len_str1, len_str2, len_str3, len_str4;
		cout << "Name" << "  " << "Address" << "  " << "TelNumber" << "  " << "Password" << endl << endl;
		while (SQLFetch(hstmt) != SQL_NO_DATA)
		{
			SQLGetData(hstmt, 1, SQL_C_CHAR, str1, sizeof(str1), &len_str1);
			SQLGetData(hstmt, 2, SQL_C_CHAR, str2, sizeof(str2), &len_str2);
			SQLGetData(hstmt, 3, SQL_C_CHAR, str3, sizeof(str3), &len_str3);
			SQLGetData(hstmt, 4, SQL_C_CHAR, str4, sizeof(str4), &len_str4);
			cout << str1 << "  " << str2 << "   " << str3 << "   " << str4 << endl;
		}
	}
	else {
		cout << "Query failed!" << endl;
		handleResult(hstmt, SQL_HANDLE_STMT, ret);
		error(ret);
	}
	ret = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
	free();
	cout << endl << endl;
	Action_Admin();
}

void Inquire_Order() {
	ret = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
	SQLWCHAR sql1[] = L"SELECT * FROM orders";
	ret = SQLExecDirect(hstmt, sql1, SQL_NTS);
	cout << "OrderNumber" << "  " << "ProductID" << "  " << "Amount" << "  " << "TelNumber" << endl << endl;
	if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO)
	{
		SQLCHAR str1[50], str2[50], str3[50], str4[50];
		SQLLEN len_str1, len_str2, len_str3, len_str4;

		while (SQLFetch(hstmt) != SQL_NO_DATA)
		{
			SQLGetData(hstmt, 1, SQL_C_CHAR, str1, sizeof(str1), &len_str1);
			SQLGetData(hstmt, 2, SQL_C_CHAR, str2, sizeof(str2), &len_str2);
			SQLGetData(hstmt, 3, SQL_C_CHAR, str3, sizeof(str3), &len_str3);
			SQLGetData(hstmt, 4, SQL_C_CHAR, str4, sizeof(str4), &len_str4);
			cout << str1 << "   " << str2 << "   " << str3 << "   " << str4 << endl;
		}
	}
	else {
		cout << "Query failed!" << endl;
		handleResult(hstmt, SQL_HANDLE_STMT, ret);
		error(ret);
	}
	ret = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
	free();
	cout << endl << endl;
	Action_Admin();
}

void Inquire_Product() {
	ret = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
	SQLWCHAR sql1[] = L"SELECT * FROM products";
	ret = SQLExecDirect(hstmt, sql1, SQL_NTS);
	cout << "ProductID" << "     " << "ProductName" << "     " << "Price" << "     " << "Quantity" << endl << endl;
	if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO)
	{
		SQLCHAR str1[50], str2[50], str3[50], str4[50];
		SQLLEN len_str1, len_str2, len_str3, len_str4;

			while (SQLFetch(hstmt) != SQL_NO_DATA)
		{
			SQLGetData(hstmt, 1, SQL_C_CHAR, str1, sizeof(str1), &len_str1);
			SQLGetData(hstmt, 2, SQL_C_CHAR, str2, sizeof(str2), &len_str2);
			SQLGetData(hstmt, 3, SQL_C_CHAR, str3, sizeof(str3), &len_str3);
			SQLGetData(hstmt, 4, SQL_C_CHAR, str4, sizeof(str4), &len_str4);
			cout << str1 << "   " << str2 << "   " << str3 << "   " << str4 << endl;
		}
	}
	else {
		cout << "Query failed!" << endl;
		handleResult(hstmt, SQL_HANDLE_STMT, ret);
		error(ret);
	}
	ret = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
	free();
	cout << endl << endl;
}

//顾客查询自己的订单
void Inquire_Order_Customer() {
	ret = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
	string str1 = "SELECT * FROM orders WHERE ctel = ";
	cout << "Please enter the telephone number." << endl;
	string ctel;
	cin >> ctel;
	string str2 = str1 + ctel ;
	wchar_t* wc = new wchar_t[str2.size()];
	swprintf(wc, 100, L"%S", str2.c_str());
	ret = SQLExecDirect(hstmt, wc, SQL_NTS);
	cout << "OrderNumber" << "  " << "ProductID" << "  " << "Amount" << "  " << "TelNumber" << endl << endl;
	if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO)
	{
		SQLCHAR str1[50], str2[50], str3[50], str4[50];
		SQLLEN len_str1, len_str2, len_str3, len_str4;
		while (SQLFetch(hstmt) != SQL_NO_DATA)
		{
			SQLGetData(hstmt, 1, SQL_C_CHAR, str1, sizeof(str1), &len_str1);
			SQLGetData(hstmt, 2, SQL_C_CHAR, str2, sizeof(str2), &len_str2);
			SQLGetData(hstmt, 3, SQL_C_CHAR, str3, sizeof(str3), &len_str3);
			SQLGetData(hstmt, 4, SQL_C_CHAR, str4, sizeof(str4), &len_str4);
			cout << str1 << "   " << str2 << "   " << str3 << "   " << str4 << endl;
		}
	}
	else {
		cout << "Query failed!" << endl;
		handleResult(hstmt, SQL_HANDLE_STMT, ret);
		error(ret);
	}
	ret = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
	free();
	cout << endl << endl;
	Action_Customer();
}


//插入
void Insert_Admin() {
	//sql语句：INSERT INTO admin VALUES (103,345)
	ret = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
	string str1 = "INSERT INTO admin VALUES (";
	cout << "Please enter the administrator number and password in sequence." << endl;
	string anum, acode;
	cin >> anum >> acode;
	string str2 = str1 + anum + "," + acode + ")";
	wchar_t* wc = new wchar_t[str2.size()];
	swprintf(wc, 100, L"%S", str2.c_str());
	ret = SQLExecDirect(hstmt, wc, SQL_NTS);
	if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
		cout << "Administrator added successfully!" << endl;
	}
	else {
		cout << "Failed to add administrator! Or administrator number already exists." << endl;
		handleResult(hstmt, SQL_HANDLE_STMT, ret);
		error(ret);
	}
	free();
	cout << endl << endl;
	Action_Admin();
}

void Insert_Customer() {
	//sql语句：INSERT INTO customer VALUES ('John','D7-209',123456,124)
	ret = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
	string str1 = "INSERT INTO customer VALUES ('";
	cout << "Please enter your name, address, telephone number and password in sequence." << endl;
	string cname, cplace,ctel,ccode;
	cin >> cname >> cplace >> ctel >> ccode;
	string str2 = str1 + cname + "','" + cplace + "'," + ctel +"," + ccode + ")";
	wchar_t* wc = new wchar_t[str2.size()];
	swprintf(wc, 100, L"%S", str2.c_str());
	ret = SQLExecDirect(hstmt, wc, SQL_NTS);
	if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
		cout << "User added successfully!" << endl;
	}
	else {
		cout << "Failed to add user! Or user phone number already exists." << endl;
		handleResult(hstmt, SQL_HANDLE_STMT, ret);
		error(ret);
	}
	free();
	cout << endl << endl;
}

void Insert_Order() {
	//sql语句：INSERT INTO orders VALUES (106,'Pizza',20,12345)
	ret = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
	string str1 = "INSERT INTO orders VALUES (";
	cout << "Please enter the order number, order name, amount and telephone number in sequence." << endl;
	string onum, oname, money,ctel;
	cin >> onum >> oname >> money >> ctel;
	string str2 = str1 + onum + ",'" + oname + "'," + money + "'," + ctel + ")";
	wchar_t* wc = new wchar_t[str2.size()];
	swprintf(wc, 100, L"%S", str2.c_str());
	ret = SQLExecDirect(hstmt, wc, SQL_NTS);
	if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
		cout << "Order added successfully!" << endl;
	}
	else {
		cout << "Failed to add order! Or order number already exists." << endl;
		handleResult(hstmt, SQL_HANDLE_STMT, ret);
		error(ret);
	}
	free();
	cout << endl << endl;
	Action_Admin();
}

void Insert_Product()
{
	//sql语句：INSERT INTO products VALUES (20,'Buger',13,20)
	ret = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
	string str1 = "INSERT INTO products VALUES (";
	cout << "Please enter the product id, name, price and quantity in sequence." << endl;
	string pid, pname, pprice, quantity;
	cin >> pid >> pname >> pprice >> quantity;
	string str2 = str1 + pid + ",'" + pname + "'," + pprice + "," + quantity + ")";
	wchar_t* wc = new wchar_t[str2.size()];
	swprintf(wc, 100, L"%S", str2.c_str());
	ret = SQLExecDirect(hstmt, wc, SQL_NTS);
	if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
		cout << "Goods on the shelves successfully!" << endl;
	}
	else {
		cout << "Failed to put the goods on the shelf! Or product name already exists." << endl;
		handleResult(hstmt, SQL_HANDLE_STMT, ret);
		error(ret);
	}
	free();
	cout << endl << endl;
	Action_Admin();
}

//点单
void Ordering() {
	//sql语句：INSERT INTO orders ( pid, money, ctel ) VALUES (1,18,12345)
	ret = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
	cout << "Please enter the product id and telephone number in sequence." << endl;
	string pid ,ctel;
	cin >> pid >> ctel;
	string sql1 = "SELECT pprice FROM products WHERE pid = ";
	string sql2 = sql1 + pid;
	wchar_t* wc_t = new wchar_t[sql2.size()];
	swprintf(wc_t, 100, L"%S", sql2.c_str());
	ret = SQLExecDirect(hstmt, wc_t, SQL_NTS);
	SQLCHAR str[50];
	SQLLEN len_str;
	while (SQLFetch(hstmt) != SQL_NO_DATA)
	{
		SQLGetData(hstmt, 1, SQL_C_CHAR, str, sizeof(str), &len_str);
	}
	string money((const char*)str);
	cout << money << endl;
	ret = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
	string str1 = "INSERT INTO orders ( pid, money, ctel ) VALUES (";
	string str2 = str1 + pid + "," + money + "," + ctel + ")";
	wchar_t* wc1 = new wchar_t[str2.size()];
	swprintf(wc1, 100, L"%S", str2.c_str());
	ret = SQLExecDirect(hstmt, wc1, SQL_NTS);
	if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
		cout << "Order successfully!" << endl;
	}
	else {
		cout << "Order failed!" << endl;
		handleResult(hstmt, SQL_HANDLE_STMT, ret);
		error(ret);
	}
	free();
	cout << endl << endl;
	Action_Customer();
}


//删除
void Delete_Admin() {
	//sql语句：DELETE FROM admin WHERE anum ='103'
	ret = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
	string str1 = "DELETE FROM admin WHERE anum ='";
	cout << "Please enter the administrator number to delete." << endl;
	string anum;
	cin >> anum;
	string str2 = str1 + anum + "'";
	wchar_t* wc = new wchar_t[str2.size()];
	swprintf(wc, 100, L"%S", str2.c_str());
	ret = SQLExecDirect(hstmt, wc, SQL_NTS);
	if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
		cout << "Delete succeeded!" << endl;
	}
	else {
		cout << "Deletion failed!" << endl;
		handleResult(hstmt, SQL_HANDLE_STMT, ret);
		error(ret);
	}
	free();
	cout << endl << endl;
	Action_Admin();
}

void Delete_Customer()
{
	//sql语句：DELETE FROM customer WHERE ctel ='12345'
	ret = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
	string str1 = "DELETE FROM customer WHERE ctel ='";
	cout << "Please enter your mobile number." << endl;
	string ctel;
	cin >> ctel;
	string str2 = str1 + ctel + "'";
	wchar_t* wc = new wchar_t[str2.size()];
	swprintf(wc, 100, L"%S", str2.c_str());
	ret = SQLExecDirect(hstmt, wc, SQL_NTS);
	if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
		cout << "Logout succeeded!" << endl;
	}
	else {
		cout << "Logout failed!" << endl;
		handleResult(hstmt, SQL_HANDLE_STMT, ret);
		error(ret);
	}
	free();
	cout << endl << endl;
}

void Delete_Order()
{
	//sql语句：DELETE FROM orders WHERE onum ='104'
	ret = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
	string str1 = "DELETE FROM orders WHERE onum ='";
	cout << "Please enter the order number to delete." << endl;
	string onum;
	cin >> onum;
	string str2 = str1 + onum + "'";
	wchar_t* wc = new wchar_t[str2.size()];
	swprintf(wc, 100, L"%S", str2.c_str());
	ret = SQLExecDirect(hstmt, wc, SQL_NTS);
	if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
		cout << "Delete succeeded!" << endl;
	}
	else {
		cout << "Deletion failed!" << endl;
		handleResult(hstmt, SQL_HANDLE_STMT, ret);
		error(ret);
	}
	free();
	cout << endl << endl;
	Action_Admin();
}

void Delete_Product()
{
	//sql语句：DELETE FROM products WHERE pid = 20
	ret = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
	string str1 = "DELETE FROM products WHERE pid =";
	cout << "Please enter the id of the off shelf product." << endl;
	string pname;
	cin >> pname;
	string str2 = str1 + pname;
	wchar_t* wc = new wchar_t[str2.size()];
	swprintf(wc, 100, L"%S", str2.c_str());
	ret = SQLExecDirect(hstmt, wc, SQL_NTS);
	if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
		cout << "Off the shelf successfully!" << endl;
	}
	else {
		cout << "Failed to get off the shelf!" << endl;
		handleResult(hstmt, SQL_HANDLE_STMT, ret);
		error(ret);
	}
	free();
	cout << endl << endl;
	Action_Admin();
} 


//修改
void Change_Admin() {
	//sql语句：UPDATE admin SET acode = 12345 WHERE anum = 101
	ret = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
	string str1 = "UPDATE admin SET acode =";
	cout << "Please enter your new password and administrator number." << endl;
	string acode, anum;
	cin >> acode >> anum;
	string str2 = str1 + acode + " WHERE anum =" + anum;
	wchar_t* wc = new wchar_t[str2.size()];
	swprintf(wc, 100, L"%S", str2.c_str());
	ret = SQLExecDirect(hstmt, wc, SQL_NTS);
	if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
		cout << "Administrator modification succeeded!" << endl;
	}
	else {
		cout << "Administrator modification failed!" << endl;
		handleResult(hstmt, SQL_HANDLE_STMT, ret);
		error(ret);
	}
	free();
	cout << endl << endl;
	Action_Admin();
}

void Change_Customer() {
	//sql语句：UPDATE customer SET cname = '李阳', cplace = 'D8-103', ccode = 5678 WHERE ctel = 56789
	ret = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
	string str1 = "UPDATE customer SET cname = '";
	cout << "Please enter your name, address, password and telephone number in sequence. (The telephone number cannot be modified, please register separately)" << endl;
	string cname, cplace, ccode, ctel;
	cin >> cname >> cplace >> ccode >> ctel;
	string str2 = str1 + cname + "', cplace = '" + cplace + "', ccode =" + ccode + " WHERE ctel = " + ctel;
	wchar_t* wc = new wchar_t[str2.size()];
	swprintf(wc, 100, L"%S", str2.c_str());
	ret = SQLExecDirect(hstmt, wc, SQL_NTS);
	if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
		cout << "User modification succeeded!" << endl;
	}
	else {
		cout << "User modification failed!" << endl;
		handleResult(hstmt, SQL_HANDLE_STMT, ret);
		error(ret);
	}
	free();
	cout << endl << endl;
	Action_Admin();
}

void Change_Order() {
	//sql语句：UPDATE orders SET oname = 'Dumplings', money = 16, ctel = 456789 WHERE onum = 102
	ret = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
	string str1 = "UPDATE orders SET oname = '";
	cout << "Please enter the order name, amount, telephone number and order number in sequence." << endl;
	string oname, money, ctel, onum;
	cin >> oname >> money >> ctel >> onum;
	string str2 = str1 + oname + "', money = " + money + ", ctel = " + ctel + " WHERE onum = " + onum;
	wchar_t* wc = new wchar_t[str2.size()];
	swprintf(wc, 100, L"%S", str2.c_str());
	ret = SQLExecDirect(hstmt, wc, SQL_NTS);
	if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
		cout << "Order modification succeeded!" << endl;
	}
	else {
		cout << "Order modification failed!" << endl;
		handleResult(hstmt, SQL_HANDLE_STMT, ret);
		error(ret);
	}
	free();
	cout << endl << endl;
	Action_Admin();
}

void Change_Product() {
	//sql语句：UPDATE products SET pname = 'buger', pprice = 14, quantity=20 WHERE pid = 10
	ret = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
	string str1 = "UPDATE products SET pname = '";
	cout << "Please enter new name, price, quantity and id." << endl;
	string pprice, pname, quantity, pid;
	cin >> pname >> pprice >> quantity >> pid;
	string str2 = str1 + pname + "'," + "pprice=" + pprice + "," + "quantity=" + quantity + " WHERE pid = " + pid;
	wchar_t* wc = new wchar_t[str2.size()];
	swprintf(wc, 100, L"%S", str2.c_str());
	ret = SQLExecDirect(hstmt, wc, SQL_NTS);
	if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
		cout << "Products modification succeeded!" << endl;
	}
	else {
		cout << "Products modification failed!" << endl;
		handleResult(hstmt, SQL_HANDLE_STMT, ret);
		error(ret);
	}
	free();
	cout << endl << endl;
	Action_Admin();
}


//登录
void Action_Admin();
void Action_Customer();

void AdminLogin() {
	//SQL:SELECT * FROM admin WHERE anum = 101 AND acode = 123
	ret = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
	string str01 = "SELECT * FROM admin WHERE anum = ";
	cout << "Please enter the administrator number and password." << endl;
	string anum, acode;
	cin >> anum >> acode;
	string str02 = str01 + anum + " AND acode = " + acode;
	wchar_t* wc = new wchar_t[str02.size()];
	swprintf(wc, 100, L"%S", str02.c_str());
	ret = SQLExecDirect(hstmt, wc, SQL_NTS);
	if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {

		SQLCHAR str1[50], str2[50];
		SQLLEN len_str1, len_str2;
		while (SQLFetch(hstmt) != SQL_NO_DATA)
		{
			SQLGetData(hstmt, 1, SQL_C_CHAR, str1, sizeof(str1), &len_str1);
			SQLGetData(hstmt, 2, SQL_C_CHAR, str2, sizeof(str2), &len_str2);
		}
		if ((const char*)str1 == anum && (const char*)str2 == acode) {
			cout << "Login succeeded!" << endl << endl;
			free();
			Action_Admin();
		}
		else {
			cout << "No such person found!" << endl;
			AdminLogin();
		}
	}
	else {
		cout << "Login failed!" << endl;
		handleResult(hstmt, SQL_HANDLE_STMT, ret);
		error(ret);
		AdminLogin();
	}
	free();
}

void CustomerLogin() {
	//SQL:SELECT * FROM customer WHERE ctel = 12345 AND ccode = 123
	ret = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
	string str01 = "SELECT * FROM customer WHERE ctel = ";
	cout << "Please enter the customer tel and password." << endl;
	string ctel, ccode;
	cin >> ctel >> ccode;
	string str02 = str01 + ctel + " AND ccode = " + ccode;
	wchar_t* wc = new wchar_t[str02.size()];
	swprintf(wc, 100, L"%S", str02.c_str());
	ret = SQLExecDirect(hstmt, wc, SQL_NTS);
	if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO && ret != NULL) {

		SQLCHAR str1[50], str2[50], str3[50], str4[50];
		SQLLEN len_str1, len_str2, len_str3, len_str4;
		while (SQLFetch(hstmt) != SQL_NO_DATA)
		{
			SQLGetData(hstmt, 1, SQL_C_CHAR, str1, sizeof(str1), &len_str1);
			SQLGetData(hstmt, 2, SQL_C_CHAR, str2, sizeof(str2), &len_str2);
			SQLGetData(hstmt, 3, SQL_C_CHAR, str3, sizeof(str3), &len_str3);
			SQLGetData(hstmt, 4, SQL_C_CHAR, str4, sizeof(str4), &len_str4);
		}
		if ((const char*)str3 == ctel && (const char*)str4 == ccode) {
			cout << "Login succeeded!" << endl << endl;
			free();
			Action_Customer();
		}
		else {
			cout << "No such person found!" << endl;
			CustomerLogin();
		}
	}
	else {
		cout << "Login failed!" << endl;
		handleResult(hstmt, SQL_HANDLE_STMT, ret);
		error(ret);
		CustomerLogin();
	}
	free();
}

//界面
void MenuAJudge();
void MenuCJudge();
void Check_Admin();
void Check_Customer();
void Check_Order();
void Check_Product();

void MainMenu() {
	cout << "---------Welcome To The Ordering System----------" << endl;
	cout << "-------------------------------------------------" << endl;
	cout << "------Please Enter The Number To Proceed---------" << endl;
	cout << "-------------------------------------------------" << endl;
	cout << "------1.Customer        -------------------------" << endl;
	cout << "------2.Admin           -------------------------" << endl;
	cout << "------0.Exit            -------------------------" << endl;
	cout << "-------------------------------------------------" << endl;
	cout << "-------------------------------------------------" << endl;

	int n;
	cin >> n;
	if (n == 1) {
		system("cls");
		MenuCJudge();

	}
	else if (n == 2) {
		system("cls");
		MenuAJudge();
	}
	else if (n == 0) {
		exit;
	}
	else {
		system("cls");
		cout << "Input error, please re-enter." << endl << endl;
		MainMenu();
	}
}

void Action_Admin() {
	cout << "---------Welcome To The Ordering System----------" << endl;
	cout << "-------------------------------------------------" << endl;
	cout << "------Please Enter The Number To Proceed---------" << endl;
	cout << "-------------------------------------------------" << endl;
	cout << "------1.Change Admin    -------------------------" << endl;
	cout << "------2.Change Customer -------------------------" << endl;
	cout << "------3.Change Order    -------------------------" << endl;
	cout << "------4.Change Product  -------------------------" << endl;
	cout << "------0.Exit            -------------------------" << endl;
	cout << "-------------------------------------------------" << endl;
	cout << "-------------------------------------------------" << endl;

	int n;
	cin >> n;
	if (n == 1) {
		system("cls");
		Check_Admin();
	} 
	else if (n == 2) {
		system("cls");
		Check_Customer();
	}
	else if (n == 3) {
		system("cls");
		Check_Order();
	}
	else if (n == 4) {
		system("cls");
		Check_Product();
	}
	else if (n == 0) {
		system("cls");
		MainMenu(); 
	}
	else {
		system("cls");
		cout << "Input error, please re-enter." << endl << endl;
		Action_Admin();
	}
}

void Action_Customer() {
	cout << "---------Welcome To The Ordering System----------" << endl;
	cout << "-------------------------------------------------" << endl;
	cout << "------Please Enter The Number To Proceed---------" << endl;
	cout << "-------------------------------------------------" << endl;
	cout << "------1.Cancellation    -------------------------" << endl;
	cout << "------2.Order           -------------------------" << endl;
	cout << "------3.Check Order     -------------------------" << endl;
	cout << "------0.Exit            -------------------------" << endl;
	cout << "-------------------------------------------------" << endl;
	cout << "-------------------------------------------------" << endl;

	int n;
	cin >> n;
	if (n == 1) {
		system("cls");
		Delete_Customer();
		MainMenu();
	}
	else if (n == 2) {
		system("cls");
		Inquire_Product();
		Ordering();
	}
	else if (n == 3) {
		system("cls");
		Inquire_Order_Customer();
	}
	else if (n == 0) {
		system("cls");
		MainMenu();
	}
	else {
		system("cls");
		cout << "Input error, please re-enter." << endl << endl;
		Action_Customer();
	}
}

void MenuAJudge() {
	cout << "---------Welcome To The Ordering System----------" << endl;
	cout << "-------------------------------------------------" << endl;
	cout << "------Please Enter The Number To Proceed---------" << endl;
	cout << "-------------------------------------------------" << endl;
	cout << "------1.Login           -------------------------" << endl;
	cout << "------0.Exit            -------------------------" << endl;
	cout << "-------------------------------------------------" << endl;
	cout << "-------------------------------------------------" << endl;

	int n;
	cin >> n;
	if (n == 1) {
		system("cls");
		AdminLogin();
	}
	else if (n == 0) {
		system("cls");
		MainMenu();
	}
	else {
		system("cls");
		cout << "Input error, please re-enter." << endl << endl;
		MenuAJudge();
	}
}

void MenuCJudge() {
	cout << "---------Welcome To The Ordering System----------" << endl;
	cout << "-------------------------------------------------" << endl;
	cout << "------Please Enter The Number To Proceed---------" << endl;
	cout << "-------------------------------------------------" << endl;
	cout << "------1.Register        -------------------------" << endl;
	cout << "------2.Login           -------------------------" << endl;
	cout << "------0.Exit            -------------------------" << endl;
	cout << "-------------------------------------------------" << endl;
	cout << "-------------------------------------------------" << endl;

	int a;
	cin >> a;
	if (a == 1) {
		system("cls");
		Insert_Customer();
		Action_Customer();
	}
	else if (a == 2) {
		system("cls");
		CustomerLogin();
	}
	else if (a == 0) {
		system("cls");
		MainMenu();
	}
	else {
		system("cls");
		cout << "Input error, please re-enter." << endl << endl;
		MenuCJudge();
	}
}

void Check_Admin() {
	cout << "---------Welcome To The Ordering System----------" << endl;
	cout << "-------------------------------------------------" << endl;
	cout << "------Please Enter The Number To Proceed---------" << endl;
	cout << "-------------------------------------------------" << endl;
	cout << "------1.ADD Admin       -------------------------" << endl;
	cout << "------2.Delete Admin    -------------------------" << endl;
	cout << "------3.Inquier Admin   -------------------------" << endl;
	cout << "------4.Change Admin    -------------------------" << endl;
	cout << "------0.Exit            -------------------------" << endl;
	cout << "-------------------------------------------------" << endl;
	cout << "-------------------------------------------------" << endl;

	int n;
	cin >> n;
	if (n == 1) {
		system("cls");
		Insert_Admin();
	}
	else if (n == 2) {
		system("cls");
		Delete_Admin();
	}
	else if (n == 3) {
		system("cls");
		Inquire_Admin();
	}
	else if (n == 4) {
		system("cls");
		Change_Admin();
	}
	else if (n == 0) {
		system("cls");
		Action_Admin();
	}
	else {
		system("cls");
		cout << "Input error, please re-enter." << endl << endl;
		Check_Admin();
	}
}

void Check_Customer() {
	cout << "---------Welcome To The Ordering System----------" << endl;
	cout << "-------------------------------------------------" << endl;
	cout << "------Please Enter The Number To Proceed---------" << endl;
	cout << "-------------------------------------------------" << endl;
	cout << "------1.ADD Customer    -------------------------" << endl;
	cout << "------2.Delete Customer -------------------------" << endl;
	cout << "------3.Inquier Customer-------------------------" << endl;
	cout << "------4.Change Customer -------------------------" << endl;
	cout << "------0.Exit            -------------------------" << endl;
	cout << "-------------------------------------------------" << endl;
	cout << "-------------------------------------------------" << endl;

	int n;
	cin >> n;
	if (n == 1) {
		system("cls");
		Insert_Customer();
		Action_Admin();
	}
	else if (n == 2) {
		system("cls");
		Delete_Customer();
		Action_Admin();
	}
	else if (n == 3) {
		system("cls");
		Inquire_Customer();
	}
	else if (n == 4) {
		system("cls");
		Change_Customer();
	}
	else if (n == 0) {
		system("cls");
		Action_Admin();
	}
	else {
		system("cls");
		cout << "Input error, please re-enter." << endl << endl;
		Check_Customer();
	}
}

void Check_Order() {
	cout << "---------Welcome To The Ordering System----------" << endl;
	cout << "-------------------------------------------------" << endl;
	cout << "------Please Enter The Number To Proceed---------" << endl;
	cout << "-------------------------------------------------" << endl;
	cout << "------1.ADD Order       -------------------------" << endl;
	cout << "------2.Delete Order    -------------------------" << endl;
	cout << "------3.Inquier Order   -------------------------" << endl;
	cout << "------4.Change Order    -------------------------" << endl;
	cout << "------0.Exit            -------------------------" << endl;
	cout << "-------------------------------------------------" << endl;
	cout << "-------------------------------------------------" << endl;

	int n;
	cin >> n;
	if (n == 1) {
		system("cls");
		Insert_Order();
	}
	else if (n == 2) {
		system("cls");
		Delete_Order();
	}
	else if (n == 3) {
		system("cls");
		Inquire_Order();
	}
	else if (n == 4) {
		system("cls");
		Change_Order();
	}
	else if (n == 0) {
		system("cls");
		Action_Admin();
	}
	else {
		system("cls");
		cout << "Input error, please re-enter." << endl << endl;
		Check_Order();
	}

}

void Check_Product() {
	cout << "---------Welcome To The Ordering System----------" << endl;
	cout << "-------------------------------------------------" << endl;
	cout << "------Please Enter The Number To Proceed---------" << endl;
	cout << "-------------------------------------------------" << endl;
	cout << "------1.ADD Product     -------------------------" << endl;
	cout << "------2.Delete Product  -------------------------" << endl;
	cout << "------3.Inquier Product -------------------------" << endl;
	cout << "------4.Change Product  -------------------------" << endl;
	cout << "------0.Exit            -------------------------" << endl;
	cout << "-------------------------------------------------" << endl;
	cout << "-------------------------------------------------" << endl;

	int n;
	cin >> n;
	if (n == 1) {
		system("cls");
		Insert_Product();
	}
	else if (n == 2) {
		system("cls");
		Delete_Product();
	}
	else if (n == 3) {
		system("cls");
		Inquire_Product();
		Action_Admin();
	}
	else if (n == 4) {
		system("cls");
		Change_Product();
	}
	else if (n == 0) {
		system("cls");
		Action_Admin();
	}
	else {
		system("cls");
		cout << "Input error, please re-enter." << endl << endl;
		Check_Product();
	}
}


int main() {
	Link();
	MainMenu();
	Unlink();
	return 0;
}
