#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include "njson.h"


int nj_ismatch_char(const char c1, const char c2){
	switch(c1){
		case '"':
			return c2 == '"';
		case '{':
			return c2 == '}';
		case '[':
			return c2 == ']';
	}
	return 0;
}
long natol(const char *nptr, int len){
	int c; /* current char */  
	long total = 0; /* current total */  
	int sign = '+'; /* if ''-'', then negative, otherwise positive */  

	c = (int)(unsigned char)*nptr;  
	if (c == '-' || c == '+')  {
		sign = c;
		c = (int)(unsigned char)*nptr++; /* skip sign */  
		len--;
	}


	while (isdigit(c) && len>0) {  
		total = 10 * total + (c - '0'); /* accumulate digit */  
		c = (int)(unsigned char)*(++nptr); /* get next char */  
		len--;
	}  

	if (sign == '-')  
		return -total;  
	else  
		return total; /* return result, negated if necessary */  
}

int natoi(const char *nptr, int len){
	return (int)natol(nptr,len);
}

double natod(const char *sptr,int len){
	double temp=10;
	double ans=0;
	char sign='+';
	if(*sptr=='-') {
		sign='-';
		sptr++;
		len--;
	} else if(*sptr=='+') {
		sptr++;
		len--;
	}

	while(*sptr!='\0' && len>0){
		if(*sptr=='.'){
			sptr++;
			len--;
			break;
		}
		ans=ans*10+(*sptr-'0');
		sptr++;
		len--;
	}
	while(*sptr!='\0' && len>0){
		ans=ans+(*sptr-'0')/temp;
		temp*=10;
		sptr++;
		len--;
	}
	if (sign == '+') 
		return ans;
	else 
		return ans*(-1);
}

const char *nj_strchr(const t_nstr *s1, char c){

	assert(s1 != NULL);
	assert(s1->pos !=NULL);

	const char *s = s1->pos;
	
	for(int i=0; i<s1->len; i++){
		if (*s == c )
			return s;
		s++;
	}
    return NULL;  
}

const char * nj_strstr(const t_nstr *s1, const t_nstr *s2){
	const char *p = s1->pos;
	int len1 = s1->len;
	int n=0;

	if(!s2->len || !*(s2->pos))
		return p;

	while(*p && len1>0){
		for(n=0; n<s2->len && *(s2->pos+n)!='\0'; n++){
			if(*(p+n) != *(s2->pos +n))
				break;
		}
		if(n == s2->len || *(s2->pos+n) == '\0') //match
			return p;
		p++;
		len1--;
	}
	return NULL;
}

static int nj_get_v(t_nstr *json, t_nstr *token, t_nstr *res){
	const char *p;
	char stack[NJ_MAX_DEPTH];		//嵌套层级
	int stack_top=0;
	int json_len=json->len;

	memset(stack,0,sizeof(stack));

	p = nj_strstr(json,token);
	if(!p)
		return ERR_NJ_NOVAL;
	p += token->len;
	json_len -= p - json->pos; 

	// p point to next char of token
	while(isspace(*p++))
		json_len--;	//filter space

	if(*p++ != ':')
		return ERR_NJ_FORMAT;			//error json format
	json_len--;

	const char * tail;
	char match_bracket;
	char bracket;

	res->pos = p;
	res->len = 0;
	stack[stack_top++]=*p++;	//"[{
	json_len--;

	while(json_len > 0){
		switch(*p){
			case '{':
			case '[':
				if(stack[stack_top - 1] != '"'){
					stack[stack_top++] = *p;
				}
				break;
			case '}':
			case ']':
				if(nj_ismatch_char(stack[stack_top-1], *p)){
					stack_top--;
					if(stack_top == 0){ //done
						res->len = p-res->pos+1;
						return ERR_NJ_OK;
					}
				}
				break;
			case '"':
				if(*(p-1) != '\\'){
					if (stack[stack_top-1] == '"'){
						stack_top--;
						if(stack_top == 0){ //done
							res->len = p-res->pos+1;
							return ERR_NJ_OK;
						}
					}else{
						stack[stack_top++]=*p;
					}
				}
				break;
			case ',':
				if (stack_top == 1 && strchr("\"[{",stack[0]) == 0){
					res->len = p - res->pos;
					return ERR_NJ_OK;
				}
				break;
			default:
				break;
		}
		p++;
		json_len--;
	}
	return ERR_NJ_FORMAT;
}


//从形如 "token[23]",返回数值
int nj_get_bracket_int(t_nstr *s1){
	assert(*(s1->pos + s1->len -1) == ']');

	const char *pos1= nj_strchr(s1,'[');
	const char *pos2= s1->pos + s1->len;

	return natoi(pos1 + 1, pos2 - pos1);
}

//从形如"[1,2]" 或者["12","23"],或者[{"xx":"ss","xx":1},{"xx",}]中获取值
//找到第index个,号，且,不在配对中
int nj_get_ar_element(const t_nstr *s1, int index, t_nstr *res){

	assert(*s1->pos == '[');
	assert(*(s1->pos + s1->len -1) == ']');

	char stack[NJ_MAX_DEPTH];
	char stack_top=0;
	memset(stack,0,sizeof(stack));

	const char *p = s1->pos+1;
	const char *last_p = p;
	int len = s1->len -1;

	while(len > 0){
		if ((*p == '[' || *p == '{') && *(p-1)!='\\'){
			stack[stack_top++]=*p;
		}else if((*p == ']' || *p == '}') && 
				nj_ismatch_char(*p,stack[stack_top-1]) && 
				*(p-1)!='\\'){
			stack_top--;
		}else if(*p == '"' && *(p-1) != '\\'){
			if (stack_top == 0)
				stack[stack_top++]=*p;
			else if(stack[stack_top-1] != '"')
				stack[stack_top++]=*p;
			else
				stack_top--;
		}else if((*p == ',' || len ==1) && stack_top == 0){
			if(index == 0){
				//found
				res->pos = last_p+1;
				res->len = p-res->pos;
				return ERR_NJ_OK;
			}
			index --;
			last_p=p;
		}
		p++;
		len--;
	}
	return ERR_NJ_FORMAT;
}

//将形如 token1.token2.token3拆分
// token形式: "people.students[2].name"
int nj_get(const char * s_json, const char * s_token,  t_nstr *p_res){
	const char * p_token=s_token;
	t_nstr sub_token;

	p_res->pos = s_json;
	p_res->len = strlen(s_json);

	sub_token.pos = s_token;
	sub_token.len=0;

	while(1){
		if(*p_token == '.' || *p_token == '\0'){
			int err;
			const char *ar_symb_pos_start = nj_strchr(&sub_token,'[');
			int oldlen = sub_token.len;
			int index;

			if(ar_symb_pos_start){
				index = nj_get_bracket_int(&sub_token);
				sub_token.len = ar_symb_pos_start - sub_token.pos;
			}

			t_nstr interm;
			if (err = nj_get_v(p_res,&sub_token, &interm) != ERR_NJ_OK)
				return err;
			
			if(ar_symb_pos_start){
				sub_token.len = oldlen;
				if(err = nj_get_ar_element(&interm, index, p_res) != ERR_NJ_OK)
					return err;
			}else{
				p_res->pos = interm.pos;
				p_res->len = interm.len;
			}

			if(*p_token == '\0')	//exit here
				return err;

			sub_token.pos = sub_token.pos + sub_token.len +1;
			sub_token.len = 0;
		}else
			sub_token.len++;

		p_token++;
	}
}

int nj_int_v(t_nstr *p_res){
	return natoi(p_res->pos,p_res->len);
}
long nj_long_v(t_nstr *p_res){
	return natol(p_res->pos,p_res->len);
}
double nj_double_v(t_nstr *p_res){
	return natod(p_res->pos,p_res->len);
}

//remove double quota
int nj_nstr_v(t_nstr *src, t_nstr *dst){
	assert(src->pos[0] == '"');
	assert(src->pos[src->len-1] == '"');

	dst->pos = src->pos + 1;
	dst->len = src->len - 2;

	return ERR_NJ_OK;
}

int nj_str_cp(t_nstr *src, char *dst, int max_len){
	assert(src->pos[0] == '"');
	assert(src->pos[src->len-1] == '"');

	const char *p = src->pos + 1;
	for(int n=0; n < src->len-2 && n < max_len-1; n++){
		*dst++=*p++;
	}
	*dst = '\0';
	return ERR_NJ_OK;
}

int nj_str2str(const t_nstr *src, char *dst, int max_len){
	const char *p=src->pos;
	for(int n=0; n<src->len && n<max_len-1; n++){
		*dst++=*p++;
	}
	*dst = '\0';
	return ERR_NJ_OK;
}

void nj_printf_str(t_nstr *src){
	char s_format[10];
	sprintf(s_format,"%%.%ds\n",src->len);
	printf(s_format,src->pos);
}

void str_replace(char * s, char ori, char replacer){
	char *p=s;
	while(*p != '\0'){
		if(*p == ori)
			*p=replacer;
		p++;
	}
}

