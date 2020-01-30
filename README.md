# 1ctools
Small command-line tools for 1C 8 automation, OpenWatcom, old-school C++:
\
\
<b>cancel1ctask - завершение выполнения фонового задания.</b>\
Usage: cancel1ctask.exe "\<1C connection string\>" "\<TaskName\>" [-com:\<ComObjectName\>] [-verbose]\
Examples:\
cancel1ctask "srvr=""server.domain.com"";ref=""Database""" "Регламентное задание 1" -com:V82.COMConnector\
\
<b>edit1cuser - редактирование параметров пользователя.</b>\
Usage: edit1cuser.exe "\<1C connection string\>" <UserName> [-com:\<ComObjectName\>] [-enable1cauth:on|off] [-set1cpassword:\<Password\>] [-lockpassword:on|off] [-setvisible:on|off] [-enabledomainauth:on|off] [-setdomainaccount:\<Account\>] [-warnunsafe:on|off]\
Examples:\
edit1cuser "file=C:\1C;usr=""Администратор""" Пользователь -enable1cauth:on\
edit1cuser "srvr=""server.domain.com"";ref=""Database""" "Петров Иван" -enabledomainauth:on -setdomainaccount:"\\\\domain\user" -com:V82.COMConnector\
\
<b>exec1c - выполнение произвольного кода из командной строки.</b>\
Usage: exec1c.exe "\<1C connection string\>" -code:"\<1C code to execute\>"|-file:\<PathToFileWithCode\> [-com:\<ComObjectName\>]\
\
1C configuration should export the following method:\
\
Процедура Exec1C(Код) Экспорт\
        Выполнить(Код);\
КонецПроцедуры\
\
Examples:\
exec1c "file=C:\1C;usr=""Администратор""" -file:code.txt\
exec1c "srvr=""server.domain.com"";ref=""Database""" -code:"ОбщийМодуль.ВыполнитьПроцедуру();" -com:V82.COMConnector\
