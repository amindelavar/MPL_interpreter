#include <MPL/system.h>

//******************************************
Boolean str_equal(String s1, String s2) {
  uint32 len = str_length(s1);
  if (len != str_length(s2)) return false;
  while (len > 0) {
    len--;
    if (s1[len] ^ s2[len]/*OR s1[len] != s2[len]*/) return false;
  }
  return true;
}

//******************************************
String convert_to_string(String s) {
  String ret = 0;
  ret = str_append("\"", s);
  ret = char_append(ret, '\"');
  return ret;
}

//******************************************
String str_reomve_quotations(String s, String type) {
  if ((!str_ch_equal(type, 's') && !str_equal(type, "str")))return s;
  uint32 s_len = str_length(s);
  if (s_len >= 2 && s[0] == '\"' && s[s_len - 1] == '\"') {
    return str_substring(s, 1, s_len - 1);
  }
  return s;
}

//******************************************
uint32 str_length(String s) {
  if (s == NULL)return 0;
  uint32 len = 0;
  while (s[len++] != 0);
  return len - 1;
}

//******************************************
String char_append(String s, uint8 c) {
  uint32 len = str_length(s);
  String s1;
  s1 = (String) malloc(sizeof(uint8) * (len + 2));
  //---------------------------
  if (s1 != 0) {
    for (uint32 i = 0; i < len; i++) {
      s1[i] = s[i];
    }
    s1[len] = c;
    s1[len + 1] = '\0';
  }
  //printf("CCC:%s,%i,%i\n",s1,len);
  return s1;
}

//******************************************
String str_append(String s1, String s2) {
  uint32 len1 = str_length(s1);
  uint32 len2 = str_length(s2);
  String s3;
  s3 = (String) malloc(sizeof(uint8) * (len1 + len2 + 1));
  //---------------------------
  if (s3 != 0) {
    for (uint32 i = 0; i < len1; i++) {
      s3[i] = s1[i];
    }
    for (uint32 i = len1; i < len1 + len2; i++) {
      s3[i] = s2[i - len1];
    }
    s3[len1 + len2] = '\0';
  }
  //printf("CCC:%s,%i,%i\n",s3,len1,len2);
  return s3;
}

//******************************************
String char_join(str_list s, uint8 sp, uint32 size, Boolean is_remove_empty) {
  String out = "";
  for (uint32 i = 0; i < size; i++) {
    if (is_remove_empty && s[i][0] == 0)
      continue;
    if (i > 0)
      out = char_append(out, sp);
    out = str_append(out, s[i]);
  }
  return out;
}

//******************************************
uint32 char_split(String s, uint8 sp, str_list *ret, Boolean is_remove_empty) {
  if (s == 0) {
    *ret = 0;
    return 0;
  }
  uint32 ind = 0;
  uint32 len = str_length(s);
  String buf = 0;
  uint32 size = 0;
  for (uint32 i = 0; i <= len; i++) {
    if (i == len || s[i] == sp) {
      size++;
    }
  }
  (*ret) = (str_list) malloc(size * sizeof(str_list));
  for (uint32 i = 0; i <= len; i++) {
    if (i == len || s[i] == sp) {
      if (!is_remove_empty || !str_is_empty(buf)) {
        if (str_is_empty(buf))(*ret)[ind++] = 0;
        else (*ret)[ind++] = buf;
      }
      buf = 0;
      if (i == len) break;
      continue;
    }
    buf = char_append(buf, s[i]);
  }
  return ind;
}

//******************************************
void str_init(String *s, String val) {
  if (val == 0) {
    *s = 0;
    return;
  }
  //if(s!=0)free(*s);
  uint32 len = str_length(val);
  //printf("VVVVV:%s,%i\n",val,len);
  *s = (String) malloc(sizeof(uint8) * (len + 1));
  for (uint32 i = 0; i < len; i++) {
    (*s)[i] = val[i];
  }
  (*s)[len] = 0;
}

//******************************************
String str_replace(String s, String old_s, String new_s, int8 num) {
  String ret = 0;
  uint32 len = str_length(s);
  uint32 old_len = str_length(old_s);
  uint32 new_len = str_length(new_s);
  for (uint32 i = 0; i < len; i++) {
    Boolean is_exist = true;
    if (i + old_len <= len) {
      uint32 ind = 0;
      for (uint32 j = i; j < i + old_len; j++) {
        if (s[j] != old_s[ind++]) {
          is_exist = false;
          break;
        }
      }
      if (is_exist) {
        if (num == -1 || num > 0) {
          ret = str_append(ret, new_s);
          i += old_len - 1;
          continue;
        } else {
          is_exist = false;
        }
        if (num > 0)num--;
      }
    } else {
      is_exist = false;
    }
    if (!is_exist) {
      ret = char_append(ret, s[i]);
    }
  }
  //printf("PPP:%s=>%s\n",s,ret);
  return ret;
}

//******************************************
Boolean str_search(str_list list, String s, uint32 size) {
  if (s == 0)return false;
  for (uint32 i = 0; i < size; i++) {
    if (str_equal(s, list[i]))
      return true;
  }
  return false;
}
//******************************************
int32 str_search_index(str_list list, String s, uint32 size) {
  if (s == 0)return -1;
  for (uint32 i = 0; i < size; i++) {
    if (str_equal(s, list[i]))
      return i;
  }
  return -1;
}

//******************************************
Boolean str_ch_equal(String s1, uint8 s2) {
  uint32 len = str_length(s1);
  if (len != 1) return false;
  if (s1[0] != s2)return false;
  return true;
}

//******************************************
Boolean char_search(uint8 list[], uint8 c) {
  uint32 len = str_length(list);
  for (uint32 i = 0; i < len; i++) {
    if (c == list[i])
      return true;
  }
  return false;
}

//******************************************
int32 char_search_index(uint8 list[], uint8 c) {
  uint32 len = str_length(list);
  for (uint32 i = 0; i < len; i++) {
    if (c == list[i])
      return i;
  }
  return -1;
}

//******************************************
uint32 char_search_count(String s, uint8 c) {
  if (s == 0 || c == 0)return 0;
  uint32 count = 0, len = 0;
  while (s[len] != 0) {
    if (s[len] == c)count++;
    len++;
  }
  return count;
}

//******************************************
String str_from_long_int(long_int num) {
  if (num == 0) return "0";
  String ret = 0;
  Boolean is_neg = false;
  uint32 ind = 0;
  while (num > 0) {
    long_int r = num % 10;
    num /= 10;
    ret = char_append(ret, r + '0');
  }
  //reverse array
  ret = str_reverse(ret);
  return ret;
}

//******************************************
int32 str_to_int32(String s) {
  uint32 len = str_length(s);
  if (len == 0)return 0;
  int32 ret = 0;
  uint32 ten = 1;
  Boolean is_neg = false;
  if (s[0] == '-') {
    len--;
    is_neg = true;
  }
  s = str_reverse(s);
  for (uint32 i = 0; i < len; i++) {
    if (s[i] >= '0' && s[i] <= '9') {
      int8 c = s[i] - '0';
      ret += ten * c;
      //printf("LL:%c=>%i=>%i\n",s[i],c,ret);
      ten *= 10;
    }
  }
  if (is_neg) ret *= -1;
  return ret;
}
//******************************************
long_int str_to_long_int(String s) {
  uint32 len = str_length(s);
  if (len == 0)return 0;
  long_int ret = 0;
  uint32 ten = 1;
  uint32 start = 0;
  if (s[0] == '-') {
    start = 1;
  }
  s = str_reverse(s);
  for (uint32 i = 0; i < len; i++) {
    if (s[i] >= '0' && s[i] <= '9') {
      int8 c = s[i] - '0';
      ret += ten * c;
      //printf("LL:%c=>%i=>%i\n",s[i],c,ret);
      ten *= 10;
    }
  }
  return ret;
}

//******************************************
Boolean str_is_int32(String s) {
  uint32 len = str_length(s);
  if (len == 0)return false;
  for (uint32 i = 0; i < len; ++i) {
    if (i == 0 && (s[i] == '-' || s[i] == '+'))continue;
    if (s[i] >= '0' && s[i] <= '9')continue;
    return false;
  }
  return true;
}
//******************************************
Boolean str_is_bool(String s){
  if(s==0)return false;
  if(str_equal(s,"true")||str_equal(s,"false"))return true;
  return false;
}
//******************************************
Boolean str_is_num(String str) {
  uint32 len = str_length(str);
  if (len == 1 && (str[0] < '0' || str[0] > '9')) {
    return false;
  }
  Boolean is_hex = false;
  if (len > 2 && str[0] == '0' && str[1] == 'x') {
    is_hex = true;
  }
  //msg("&&EE", str, is_hex)
  for (uint32 i = 0; i < len; i++) {
    if ((str[i] >= '0' && str[i] <= '9') || str[i] == '.' || (i == 0 && (str[i] == '+' || str[i] == '-') ||
        (i == 1
            && (str[i] == 'x' || str[i] == 'b' || str[i] == 'o')))) {
      continue;
    } else if (i == len - 1 && len > 1 && (str[i] == 'i' || str[i] == 'f' || str[i] == 'd' || str[i] == 'h') &&
        (str[i - 1] >= '0' && str[i - 1] <= '9')) {
      continue;
    } else if (is_hex &&
        (str[i] == 'a' || str[i] == 'b' || str[i] == 'c' || str[i] == 'd' || str[i] == 'e' || str[i] == 'f')) {
      continue;
    } else {
      return false;
    }
  }
  return true;
}

//******************************************
String str_reverse(String s) {
  uint32 len = str_length(s);
  if (len == 0)return 0;
  String ret = (String) malloc(sizeof(uint8) * (len + 1));
  uint32 ind = len - 1;
  for (uint32 i = 0; i < len; i++) {
    ret[ind--] = s[i];
  }
  ret[len] = 0;
  return ret;
}

//******************************************
String str_trim_space(String s) {
  uint32 len = str_length(s);
  if (len == 0)return 0;
  String tmp1 = 0, tmp2 = 0, ret = 0;
  Boolean is_first = false, is_last = false;
  //trim from first
  for (uint32 i = 0; i < len; i++) {
    if (is_first || !char_search(WHITE_SPACES, s[i])) {
      is_first = true;
      tmp1 = char_append(tmp1, s[i]);
    }
  }
  //config
  len = str_length(tmp1);
  if (len == 0)return 0;
  str_init(&tmp2, tmp1);
  free(tmp1);
  tmp2 = str_reverse(tmp2);
  //trim from last
  for (uint32 i = 0; i < len; i++) {
    if (is_last || !char_search(WHITE_SPACES, tmp2[i])) {
      is_last = true;
      ret = char_append(ret, tmp2[i]);
    }
  }
  //config
  ret = str_reverse(ret);

  return ret;
}

//******************************************
String char_to_str(uint8 c) {
  String s = 0;
  return char_append(s, c);
}

//******************************************


//******************************************
uint32 str_list_size(str_list s) {
  if (s == 0)return 0;
  uint32 len = 0;
  for (;;) {
    if (s[len++] == NULL) break;
  }
  return len - 1;
}

//******************************************
void str_empty(String *s) {
  if (s == 0 || !*s)return;
  //printf("$RR:%s,%i,%i\n",*s,*s,s);
  //free(*s); //is correct
  *s = NULL;
}

//******************************************
Boolean str_is_empty(String s) {
  if (s == 0) return true;
  if (s[0] == 0) return true;
  return false;
}

//******************************************
void str_swap(String *s1, String *s2) {
  String s3 = 0;
  str_init(&s3, (*s1));
  str_init(&(*s1), (*s2));
  str_init(&(*s2), s3);
}

//******************************************
String str_substring(String s, uint32 start, uint32 end) {
  uint32 s_len = str_length(s);
  if (end == 0 && start > 0)end = s_len;
  if (start >= end) return 0;
  if (end > s_len)end = s_len;
  String ret = 0;
  for (uint32 i = start; i < end; i++) {
    ret = char_append(ret, s[i]);
  }
  return ret;
}

//******************************************
int32 str_indexof(String s, String s1, uint32 start) {
  if (s == 0 || s1 == 0)return -1;
  uint32 len = str_length(s);
  uint32 len1 = str_length(s1);
  if (start >= len || len1 > len)return -1;
  for (uint32 i = start; i < len; i++) {
    Boolean is_exist = true;
    if (i + len1 <= len) {
      uint32 ind = 0;
      //printf("WWWW:%i,%i,%i\n", i, i + len1, len);
      for (uint32 j = i; j < i + len1; j++) {
        if (s[j] != s1[ind++]) {
          is_exist = false;
          break;
        }
      }
      if (is_exist) {
        return i;
      }
    } else {
      is_exist = false;
    }
  }
  return -1;
}
//******************************************
int32 char_last_indexof(String s, uint8 ch) {
  uint32 len = str_length(s);
  if (len < 1)return -1;
  for (uint32 i = len - 1; i >= 0; i--) {
    if (i == -1)break;
    if (s[i] == ch) return i;
  }
  return -1;
}
//******************************************
String str_to_upper_case(String text) {
  String s = 0;
  for (; *text; text++) {
    if (('a' <= *text) && (*text <= 'z'))
      s = char_append(s, 'A' + (*text - 'a'));
    else s = char_append(s, *text);
  }
  return s;
}

//******************************************
String str_to_lower_case(String text) {
  String s = 0;
  for (; *text; text++) {
    if (('A' <= *text) && (*text <= 'Z'))
      s = char_append(s, 'a' + (*text - 'A'));
    else s = char_append(s, *text);
  }
  return s;
}

//******************************************
String str_from_int32(int32 x) {
  if (x == 0)return "0";
  Boolean is_neg = false;
  String ret = 0;
  if (x < 0) {
    x *= -1;
    is_neg = true;
  }

  while (x) {
    ret = char_append(ret, (x % 10) + '0');
    x = x / 10;
  }
  if (is_neg) ret = char_append(ret, '-');
  ret = str_reverse(ret);
  return ret;
}
//******************************************
String str_from_bool(Boolean x) {
  if (x)return "true";
  return "false";
}
//******************************************
String str_from_int64(int64 x) {
  if (x == 0)return "0";
  Boolean is_neg = false;
  String ret = 0;
  if (x < 0) {
    x *= -1;
    is_neg = true;
  }

  while (x) {
    ret = char_append(ret, (x % 10) + '0');
    x = x / 10;
  }
  if (is_neg) ret = char_append(ret, '-');
  ret = str_reverse(ret);
  return ret;
}
//******************************************
Boolean str_to_bool(String s) {
  if (str_ch_equal(s, '0') || str_equal(s, "false") || str_equal(str_to_lower_case(s), "false")) return false;
  return true;
}
//******************************************
double str_to_double(String s) {
  double val, power;
  int i, sign;
  for (i = 0; isspace(s[i]); i++); /* skip white space */

  sign = (s[i] == '-') ? -1 : 1;
  if (s[i] == '+' || s[i] == '-')
    i++;
  for (val = 0.0; isdigit(s[i]); i++) {
    val = 10.0 * val + (s[i] - '0');
  }
  if (s[i] == '.')
    i++;
  for (power = 1.0; isdigit(s[i]); i++) {
    val = 10.0 * val + (s[i] - '0');
    power *= 10;
  }
  return sign * val / power;
}

//******************************************
String str_from_double(double n, uint8 afterpoint) {
  String ret = 0;
  // Extract integer part
  int64 ipart = (int64) n;
  // convert integer part to string
  str_init(&ret, str_from_int64(ipart));
  if (n < 0) {
    ipart *= -1;
    n *= -1;
  }
  // Extract floating part
  double fpart = n - (double) ipart;
  //printf("CCCC:%f=>%lld,%s,%f\n",n,ipart,ret,fpart);
  // check for display option after point
  if (afterpoint > 0 && fpart != 0) {
    ret = char_append(ret, '.');// add dot
    double power = int32_power(10, afterpoint); //calc power of 10
    double one_unit = (double) 1 / power; //calc one unit like 0.01 or 0.0001
    fpart = (fpart * power) + one_unit;
    String fpart_s = str_from_int32((int) fpart);
    uint32 fpart_s_len = str_length(fpart_s);
    //add pre zeros like 0.005
    if (afterpoint > fpart_s_len) {
      for (uint32 i = 0; i < afterpoint - fpart_s_len; ++i) {
        ret = char_append(ret, '0');
      }
    }
    //printf("OOOOO:%f=>%f,%f,%i\n",n,fpart,power,afterpoint);
    ret = str_append(ret, fpart_s);
  }
  return ret;
}

//******************************************
String str_multi_append(String s, String s1, String s2, String s3, String s4, String s5) {
  String ret = 0;
  str_init(&ret, s);
  if (s1 != 0)ret = str_append(ret, s1); else return ret;
  if (s2 != 0)ret = str_append(ret, s2); else return ret;
  if (s3 != 0)ret = str_append(ret, s3); else return ret;
  if (s4 != 0)ret = str_append(ret, s4); else return ret;
  if (s5 != 0)ret = str_append(ret, s5);
  return ret;
}

//******************************************
void str_to_utf8(str_utf8 *ret, String val) {
  uint32 len = str_length(val);
  (*ret) = (str_utf8) malloc(sizeof(uint32) * (len + 1));
  for (uint32 i = 0; i < len; i++) {
    (*ret)[i] = val[i];
  }
  (*ret)[len] = 0;
}

//******************************************
String str_from_const_char(const char s[]) {
  String ret = 0;
  if (s == NULL)return 0;
  uint32 len = 0;
  while (s[len++] != 0);
  if (len == 0)return 0;
  //printf("VVVVV:%s,%i\n",val,len);
  ret = (String) malloc(sizeof(uint8) * (len + 1));
  for (uint32 i = 0; i < len; i++) {
    ret[i] = s[i];
  }
  ret[len] = 0;
  return ret;
}

//******************************************
String str_trim_optimized_boolean(String str) {
  String final = 0;
  Boolean is_string = false;
  for (uint32 i = 0; i < str_length(str); i++) {
    if (str[i] == '\"' && (i == 0 || str[i - 1] != '\\')) {
      is_string = switch_bool(is_string);
    }
    if (!is_string && str[i] != ')' && str[i] != '(' && str[i] != '!') {
      final = char_append(final, str[i]);
    }
  }
  return final;
}

//******************************************
String str_trim_number(String str) {
  String final = 0;
  Boolean is_num = false;
  uint32 str_len = str_length(str);
  for (uint32 i = 0; i < str_len; i++) {
    if ((!is_num && str[i] != '0') || is_num) {
      final = char_append(final, str[i]);
    }
    if (!is_num && ((str[i] >= '1' && str[i] <= '9') || (i + 1 < str_len && str[i] == '0' && str[i + 1] == '.'))) {
      if (str[i] == '0')final = char_append(final, str[i]);
      is_num = true;
    }
  }
  return final;
}

//******************************************
String str_trim_last_float(String str) {
  String final = 0;
  Boolean is_point = false;
  int32 point_ind = -1, zero_ind = -1;
  uint32 str_len = str_length(str);
  for (uint32 i = 0; i < str_len; i++) {
    if (str[i] == '.') {
      is_point = true;
      point_ind = i;
      break;
    }
  }
  //-------------------------
  if (!is_point) return str;
  //-------------------------
  for (int32 i = str_len - 1; i >= point_ind; i--) {
    if (str[i] != '0') {
      if (str[i] == '.') {
        zero_ind = i - 1;
      } else {
        zero_ind = i;
      }
      break;
    }
  }
  //-------------------------
  final = str_substring(str, 0, zero_ind + 1);
  return final;
}

//******************************************
Boolean str_has_suffix(String s, String find) {
  int32 ind = str_indexof(s, find, 0);
  if (ind == -1)return false;
  //printf("@#@SSSS:%s,%s[%i,%i,%i]\n",s,find,ind,ind + str_length(find) + 1,str_length(s));
  if (ind + str_length(find) == str_length(s)) return true;
  return false;
}
//******************************************
//******************************************
//******************************************
//******************************************
//******************************************
//******************************************
//******************************************
//******************************************
//******************************************
//******************************************
//******************************************

//******************************************
String str_make_negative_number(String s) {
  uint32 s_len = str_length(s);
  if (s_len > 1 && s[0] == '-')s[0] = '+';
  else if (s_len > 1 && s[0] == '+')s[0] = '-';
  else if (s_len > 0)s = str_multi_append("-", s, 0, 0, 0, 0);
  return s;
}

//******************************************
void str_detachment_float(String s, String *s1, String *s2) {
  Boolean is_point = false;
  String int_s = 0, float_s = 0;
  for (uint32 i = 0; i < str_length(s); i++) {
    if (s[i] == '.') {
      is_point = true;
      continue;
    }
    if (!is_point) {
      int_s = char_append(int_s, s[i]);
    } else {
      float_s = char_append(float_s, s[i]);
    }
  }
  if (s1 != 0)str_init(&(*s1), int_s);
  if (s2 != 0)str_init(&(*s2), float_s);
}
//******************************************
String char_backspace(String s) {
  //printf ("$BACKSPACE:%s\n", s);
  uint32 len = str_length(s);
  if (len == 0)return 0;
  s[len - 1] = 0;
  return s;
}

//******************************************
uint8 char_to_uint8(uint8 c, Boolean *has_err) {
  if (c < '0' || c > '9') {
    if (*has_err != 0)(*has_err) = true;
    return 0;
  }
  if (*has_err != 0)(*has_err) = false;
  return c - '0';
}
//******************************************
String str_join(str_list s, uint32 len, String delimiter) {
  String ret = 0;
  for (uint32 i = 0; i < len; i++) {
    ret = str_append(ret, s[i]);
    if (i + 1 < len)ret = str_append(ret, delimiter);
  }
  return ret;
}
//******************************************
String str_to_validate(String s) {
  String ret = 0;
  for (uint32 i = 0; i < str_length(s); i++) {
    if (s[i] == '\"' && (i == 0 || s[i - 1] != '\\')) {
      ret = str_append(ret, "\\\"");
    } else ret = char_append(ret, s[i]);
  }
  return ret;
}
//******************************************
String str_from_path(String path){
  String ret = 0;
  for (uint32 i = 0; i < str_length(path); i++) {
    if (path[i] == '\\' && (i == 0 || path[i - 1] != '\\')) {
      ret = str_append(ret, "\\\\");
    } else ret = char_append(ret, path[i]);
  }
  return ret;
}