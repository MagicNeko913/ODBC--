# ODBC-OrderingSystem-
通过ODBC连接SQL Server数据库，实现了在命令行内执行订餐系统的管理

注意：数据库中的中文数据输出正常，但是无法读取命令行中输入的中文字符（读取到的是乱码），请尽量使用英文

将代码中 连接 块的 TEST 换成ODBC数据源的名字，
UserName 换成SQL Server的登录名，
PassWord 换成登录名的密码（SQL Server不支持太弱的密码登录，请至少包含英文小写字符和数字且不小于8位）

环境：VS2022，SQLserver2019，Windows10 最新的SDK，在ODBC数据源中已将OrderingSystem设为默认数据库
