-----------------------------
external modules:
sqlite[OK],math,os,strs,mhl,mgl,serve,json

------------------module:mgl (mpl graphics library)
- str version()


------------------module:sqlite
- str version()					[OK]


- str LibVersion()				[OK]

- num open(str)					[OK]

- bool close(num)				[OK]

- str[?,?] exec(num,str)		[OK]

------------------module:strs
- str version()



- str StrCrop(str,num,num)

- str StrIndexof(str,str,num)

- str StrUppercase(str)

- str StrLowercase(str)

- str StrTrim(str,num)

- str StrReplace(str,str,str)

- str StrSplit(str[..],str) 

- str StrContains(str,str)

- str StrJoin(str,str)

- str StrReverse(str)

------------------module:math
- num PI()
- num E()
- str version()


- num sqrt(num) : calculate root 2 of a num

- num factorial(num)

- num sin(num)

- num cos(num)

- num tan(num)

- str eval(str) //eval("5*7+4")

- num round(num) //3=round(2.8)

- num abs(num)

- num floor(num) //2=floor(2.8)

- num log10(num)

- num acos(num)

- num asin(num)

- num atan(num)

- num sinh(num)

- num cosh(num)

- num tanh(num)

- num degrees(num)

- num radians(num)

- num ConvertBase(num,num,bool)

------------------module:os
- str version()



- str HomeDir()

- num uptime()

- str[?] OSInfo()

- str[?] CpuInfo()

- str[?] MemInfo()

- bool shutdown(num)

- bool ProcessStart(str,str[?])


------------------module:mhl (mpl hacking library)
- str version()



- DnsMap()

- DnsTrace()

- WhoisInfo()

- GatheringData(num type)