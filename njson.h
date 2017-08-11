#ifndef __NJSON_H__
#define __NJSON_H__


// define max nesting depth of json object
#define NJ_MAX_DEPTH 10

#define ERR_NJ_OK 0
#define ERR_NJ_FORMAT 1
#define ERR_NJ_NOVAL 2

// nj string type: t_nstr diff with standard cstring terminate with \0 
// t_nstr has len attribute
typedef struct _nstr{
	const char * pos;	//string point
	unsigned int len;	//string lenth
}t_nstr;

// atol with length restrict
// ex: natol("12345",2) = 12
extern long natol(const char *nptr, int len);
extern int natoi(const char *nptr, int len);
extern double natod(const char *nptr, int len);

// nj string type strchr
extern const char *nj_strchr(const t_nstr *s1, char c);
extern const char * nj_strstr(const t_nstr *s1, const t_nstr *s2);

// ex: nj_get_bracket_int({"[1234]",6}) = 1234
extern int nj_get_bracket_int(t_nstr *s1);

// get element string in a json string compose with array
// ex: nj_get_ar_element({"[11,22,3,4]",8},1) = "22"
extern int nj_get_ar_element(const t_nstr *s1, int index, t_nstr *res);


// find the element string in a json string
// nj_get("{\"prop1\":1,\"prop2\":\"some text\"}","prop1",&res);
// res = {"\"some text\"",11}
extern int nj_get(const char * s_json, const char * s_token,  t_nstr *p_res);

//type translate

//ex: nj_int_v({"123",3}) = 123
extern int nj_int_v(t_nstr *p_res);

//ex: nj_long_v({"123",3}) = 123
extern long nj_long_v(t_nstr *p_res);

//ex: nj_double_v({"12.339a",4}) = 12.33 
extern double nj_double_v(t_nstr *p_res);

//ex: nj_nstr_v({"\"ab39ab\"",8}) = {"ab39ab",6}; remove both side " from nstr
extern int nj_nstr_v(t_nstr *src, t_nstr *dst);

//remove both side " from src and copy to standard c string byte buffer;
//ex: nj_str_cp({"\"ab39ab\"",8},dst,4); dst = "ab39"
extern int nj_str_cp(t_nstr *src, char *dst, int max_len);

//copy nstr to  standard c string byte buffer;
//ex: nj_str2str({"\"ab39ab\"",8},dst,4); dst = "\"ab3"
extern int nj_str2str(const t_nstr *src, char *dst, int max_len);


//printf nstr;
extern void nj_printf_str(t_nstr *src);

extern void  str_replace(char * src, char ori, char replacer);

#endif
