
@echo off

::
:: ������� CMD ����, (Start + Cmd ����)
::
:: See: http://blog.csdn.net/iloli/article/details/44339799
::

::
:: For ����
::
:: See: http://www.jb51.net/article/72077.htm
::

::
:: Start ����
::
:: See: http://www.jb51.net/article/43571.htm
:: See: http://www.cnblogs.com/Braveliu/p/5078283.html
::

:: for /f "delims= " %%i in (iplist.txt) do echo area = %%~i
:: for /f "tokens=2 delims= " %%i in (iplist.txt) do echo ip = %%~i
:: for /f "tokens=1,2 delims= " %%a in (iplist.txt) do echo area = %%~a, ip = %%~b

for /f "tokens=1,2 delims= " %%a in (iplist.txt) do (
    start /i "%%~a" "cmd.exe" /k "cd /d c:\ && echo . && echo %%~a ping %%~b -t && ping %%~b -t"
)

:: start cmd /k "cd /d c:\ && echo ���ǵ�һ������ && pause && ping 104.224.132.45 -t"
:: start cmd /k "cd /d c:\ && echo ���ǵڶ������� && pause && ping 172.96.8.178 -t"
:: start cmd /k "cd /d c:\ && echo ���ǵ��������� && pause && ping 185.191.228.34 -t"

:: start "��ɼ�" "cmd.exe" /k "cd /d C:\Windows && echo ���ǵ�һ������ && ping 104.224.132.45 -t"
:: start "����˹" "cmd.exe" /k "cd /d C:\Windows && echo ���ǵڶ������� && ping 172.96.8.178 -t"
:: start "֥�Ӹ�" "cmd.exe" /k "cd /d C:\Windows && echo ���ǵ��������� && ping 185.191.228.34 -t"

:: start /b /i cmd /k "ping 104.224.132.45 -t"

:: start /i /b "" "cmd.exe" /k "cd /d C:\Windows && ping 104.224.132.45 -t"
:: cmd.exe /k "cd /d C:\Windows && ping 172.96.8.178 -t"
:: cmd.exe /k "cd /d C:\Windows && ping 185.191.228.34 -t"

:: start "��ɼ�" "cmd.exe" /c 'ping 104.224.132.45 -t'
:: start "����˹" "cmd.exe" /c 'ping 172.96.8.178 -t'
:: start "֥�Ӹ�" "cmd.exe" /c 'ping 185.191.228.34 -t'

:: cmd.exe /c ping 104.224.132.45 -t
:: cmd.exe /c ping 172.96.8.178 -t
:: cmd.exe /c ping 185.191.228.34 -t

:: pause
