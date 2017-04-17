
@echo off

::
:: 启动多个 CMD 窗口, (Start + Cmd 命令)
::
:: See: http://blog.csdn.net/iloli/article/details/44339799
::

::
:: For 命令
::
:: See: http://www.jb51.net/article/72077.htm
::

::
:: Start 命令
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

:: start cmd /k "cd /d c:\ && echo 这是第一个窗口 && pause && ping 104.224.132.45 -t"
:: start cmd /k "cd /d c:\ && echo 这是第二个窗口 && pause && ping 172.96.8.178 -t"
:: start cmd /k "cd /d c:\ && echo 这是第三个窗口 && pause && ping 185.191.228.34 -t"

:: start "洛杉矶" "cmd.exe" /k "cd /d C:\Windows && echo 这是第一个窗口 && ping 104.224.132.45 -t"
:: start "堪萨斯" "cmd.exe" /k "cd /d C:\Windows && echo 这是第二个窗口 && ping 172.96.8.178 -t"
:: start "芝加哥" "cmd.exe" /k "cd /d C:\Windows && echo 这是第三个窗口 && ping 185.191.228.34 -t"

:: start /b /i cmd /k "ping 104.224.132.45 -t"

:: start /i /b "" "cmd.exe" /k "cd /d C:\Windows && ping 104.224.132.45 -t"
:: cmd.exe /k "cd /d C:\Windows && ping 172.96.8.178 -t"
:: cmd.exe /k "cd /d C:\Windows && ping 185.191.228.34 -t"

:: start "洛杉矶" "cmd.exe" /c 'ping 104.224.132.45 -t'
:: start "堪萨斯" "cmd.exe" /c 'ping 172.96.8.178 -t'
:: start "芝加哥" "cmd.exe" /c 'ping 185.191.228.34 -t'

:: cmd.exe /c ping 104.224.132.45 -t
:: cmd.exe /c ping 172.96.8.178 -t
:: cmd.exe /c ping 185.191.228.34 -t

:: pause
