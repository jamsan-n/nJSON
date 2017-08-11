#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// use sleep function in test
#include <unistd.h>

#include "njson.h"

void test_type_trans(){
	double ad;
	long al;
	int ai;

	t_nstr res;

	ad = natod("32.99688",5);
	printf("32.99688 double:%f\n",ad);

	ai = natol("12345678",4);
	printf("12345678 int:%d\n",ai);

	al = natol("12345678",7);
	printf("12345678 long:%ld\n",al);
}

void test_nj_str(){
	t_nstr res;
	const char *p;

	//test nj_strstr
	t_nstr ns1={
		"abcdefghijklmn", 10,
	};
	t_nstr ns2={
		"efg", 4,
	};
	p = nj_strstr(&ns1,&ns2);
	printf("nj_strstr: %.3s\n",p);

	//test nj_get_bracket_int
	ns2.pos = "abcd[1234]";
	ns2.len = 10;
	printf("nj_get_bracket_int:%d\n",nj_get_bracket_int(&ns2));

	//test nj_get_ar_element
	const char *s2 = "[1,2,3,4,5]";
	const char *s3 = "[\"stra\",\"strb\",\"strc\"]";
	t_nstr ns3;
	ns3.pos = s2;
	ns3.len = strlen(s2);
	if(nj_get_ar_element(&ns3,3,&ns2) == ERR_NJ_OK)
		nj_printf_str(&ns2);

	ns3.pos = s3;
	ns3.len = strlen(s3);
	if(nj_get_ar_element(&ns3,1,&ns2) == ERR_NJ_OK)
		nj_printf_str(&ns2);
	if(nj_get_ar_element(&ns3,2,&ns2) == ERR_NJ_OK)
		nj_printf_str(&ns2);
}

void test_nj_json(){

	t_nstr res;

	//test nj_get
	char *s1 = "{\"stoken\":\"hello\", \"itoken\":123, \
				\"artoken1\":[1,2],\"artoken2\":[\"a\",\"bc\",\"def\"],\
				\"objtoken1\":{\"sub1\":99,\"sub2\":{\"sub21\":\"some\",\"sub22\":\"some\"}},\
				\"objtoken2\":{\"sub1\":[\"ab\",\"cd\",\"ef\"]}}";
	printf("ori json: %s\n",s1);

	if(nj_get(s1,"stoken",&res) == 0)
		nj_printf_str(&res);
	if(nj_get(s1,"itoken",&res) == 0)
		nj_printf_str(&res);
	if(nj_get(s1,"artoken1[1]",&res) == 0)
		nj_printf_str(&res);
	if(nj_get(s1,"artoken2[1]",&res) == 0)
		nj_printf_str(&res);
	if(nj_get(s1,"objtoken1.sub2.sub22",&res) == 0)
		nj_printf_str(&res);
	if(nj_get(s1,"objtoken2.sub1[1]",&res) == 0)
		nj_printf_str(&res);


	//more complex
	s1 = "{\"stoken\":\"hello\", \"itoken\":123, \
				\"artoken1\":[1,2],\"artoken2\":[\"a\",\"bc\",\"def\"],\
				\"objtoken1\":{\"sub1\":99,\"sub2\":{\"sub21\":\"some\",\"sub22\":\"some\"}},\
				\"objtoken2\":{\"sub1\":[\"ab\",\"cd\",\"ef\"]},\
				\"s_p\":{\"sub1\":{},\"sub2\":{\"complex_str\":\"[]*(p&^'fdksal{{\"},\"sub3\":[]}}";
	printf("ori json: %s\n",s1);
	if(nj_get(s1,"s_p.sub2.complex_str",&res) == 0)
		nj_printf_str(&res);
}

void example(){
	printf("====================================================================\n");
	printf("we generate json string like this\n");
	char json[128];

	// I hate the backslah before double quota in a string, so replace with ';
	char json_temple[]={"{'sub1':'%s','subint':'%d','subArray':[%d,%d,%d],'subObj':{'obj11':%d,'obj12':'%s'}}"};
	str_replace(json_temple,'\'','"');
	snprintf(json,sizeof(json),json_temple,"helloWorld",12345,100,200,300,400,"ends now");
	printf("%s\n",json);

	char res_buff[10];
	t_nstr res;
	if(nj_get(json,"subObj.obj12",&res) == ERR_NJ_OK){
		nj_str_cp(&res, res_buff, sizeof(res_buff));
		printf("get element \"subObj.obj12\" : %s\n" , res_buff);
	}else{
		printf("get element \"subObj.obj12\" fail! \n");
	}
}

int main(){
	test_type_trans();
	test_nj_str();
	test_nj_json();
	example();
	printf("\npress q enter to exit\n");
	while(getchar()!='q')
		sleep(100);
}
