# nJSON

nano JSON parser in ANSI C, suitable for lacks memory embeded MCUs like MCS51, MSP430 or more.

## Table of contents
* [License](#license)
* [Welcome to nJSON](#welcome-to-njson)
* [How to use](#how-to-use)
* [Enjoy nJSON!](#enjoy-njson)

## License

apache-2.0 license.

# welcom-to-njson 

When I wrote programs for some memory limited MCUs like mcs51, I found it's difficult to handle JSON object. Other cJSON library need dynamic allocate memory and generate whole JSON object tree in ram. But some MCU only have 512 Byte or even more less space. So I dedicate this project just for one purpose: I want pick a element's value from a JSON string, no need parse the whole object. So handle as string, it will be simple and easy.

There is no dynamic memory allocation and global variable, all we need restrict in function stack space, so never need worry about memory leakage and thread safety.

Generally we need no more then 20 bytes can work well for this lib.


# how-to-use

import njson.h and njson.c to your project

# enjoy-njson

## generate JSON string
```
	char json[64];
	// I hate the backslah before double quota in a string, so replace with ';
	char json_temple[]={"{'sub1':'%s','subint':'%d','subArray':[%d,%d,%d],'subObj':{'obj11':%d,'obj12':'%s'}}"};
	str_replace(json_temple,'\'','"');
	snprintf(json,sizeof(json),json_temple,"helloWorld",12345,100,200,300,400,"ends now");
	printf("%s\n",json);
```

## get element "subObj.obj12" value
```
	char res_buff[10];
	t_nstr res;
	nj_get(json,"subObj.obj12",&res);
	nj_str_cp(&res, res_buff, sizeof(res_buff));
	printf("get element \"subObj.obj12\" : %s\n" , res_buff);
```
***

- author: Niu Jiansheng
