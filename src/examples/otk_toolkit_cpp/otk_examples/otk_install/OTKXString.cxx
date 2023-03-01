/*
	Copyright (c) 2022 PTC Inc. and/or Its Subsidiary Companies. All Rights Reserved.
*/

 
/*
   This example will demonstrate basic operations using xstring
*/
 
#include <xstring.h>
#include <xstring_c.h>


// to run: 
// otk_examples -cip 1
extern "C" void otkXstringDemo ()
{

    // 1. xstring () & 2. xstring (cStringT str)
    {
       cStringT cstr = "otkXstringDemo";
       xstring xstr;
       xstring xstr1(cstr);
       xstr = xstr1;
       printf("1 & 2: %s\n", (const char*)xstr);
    }

    // 3. xstring (const xstring &str)
    {
       const xstring xstr = "CopyConstructor";
       xstring xstrCp(xstr);
       printf("3: %s\n", (const char*)xstrCp);
    }

    // 4. xstring (const xstring_c &str)
    {
        xrstring xrstr = "xstring_c";
        xstring_c xstrc = xstrcreate_c(xrstr);
        xstring xstr(xstrc);
        printf("4: %s\n", (const char *)xstr);
    }

    // 5. xstring (cWStringT str)
    {
       cWStringT cwstr = L"ConstWideCharArray";
       xstring xstr (cwstr);
       printf("5: %s\n", (const char*)xstr);
    }

    // 6. xstring (cStringT str1, cStringT str2)
    {
       cStringT str1 = "Str1";
       cStringT str2 = "_Str2";
       xstring xstr (str1, str2);
       printf("6: %s\n", (const char*)xstr);
    }

    // 7. xstring (cWStringT str1, cWStringT str2)
    {
       cWStringT cwstr1 = L"WStr1";
       cWStringT cwstr2 = L"_WStr2";
       xstring xstr (cwstr1, cwstr2);
       printf("7: %s\n", (const char*)xstr);
    }

    // 8. xstring (cStringT str, int len)
    {
       cStringT str = "TruncateMultiByteString";
       xstring xstr (str, 15);
       printf("8: %s\n", (const char*)xstr);
    }

    // 9. xstring (cWStringT str, int len)
    {
       cWStringT cwstr = L"TruncateWideCharString";
       xstring xstr (cwstr, 15);
       printf("9: %s\n", (const char*)xstr);
    }

    // 10. xstring (cStringT str, int from, int to)
    {
       cStringT str = "TruncateMultiByteString";
       xstring xstr (str, 2, 15);
       printf("10: %s\n", (const char*)xstr);
       xstring xstr1 (str, 2, -1);
       printf("%s\n", (const char*)xstr1);
    }

    // 11. xstring (cWStringT str, int from, int to)
    {
       cWStringT cwstr = L"TruncateWideCharString";
       xstring xstr (cwstr, 2, 15);
       printf("11: %s\n", (const char*)xstr);
       xstring xstr1 (cwstr, 2, -1);
       printf("%s\n", (const char*)xstr1);
    }

    // 12. xstring (strstream &stream) - deprecated
    {
       strstream input;
       input << "strstream" << 1 << "Input";
       xstring xstream (input);
       printf("12: %s\n", (const char*)xstream);
    }

    // 13. xstring (stringstream &stream)
    {
       stringstream input;
       input << "stringstream" << 1 << "Input";
       xstring xstream (input);
       printf("13: %s\n", (const char*)xstream);
    }

    // 14. xstring (wstringstream &stream)
    {
       wstringstream input;
       input << "wstringstream" << 1 << "Input";
       xstring xstream (input);
       printf("14: %s\n", (const char*)xstream);
    }

    xstring string = "OTK_xstring_methods";

    // 15. operator cStringT () const
    {
        const char *cstr = (const char*)string;
        printf("15: %s\n", cstr);
    }

    // 16. operator cWStringT () const
    {
        const wchar_t* wstr = (const wchar_t*)string;
        printf("16: %ws\n", wstr);
    }

    // 17. xstring_c getxstring_c () const;
    {
        xstring_c xstrc = string.getxstring_c();
        printf("17: %s\n",xstrc.mbstr);
    }

    // 18. bool IsNull () const
    {
        xstring xstr;
        bool flag = xstr.IsNull();
        if(flag)
            printf("18: xstring IsNull.\n");
    }

    // 19. bool ToInt (int *out_val) const
    {
        int integ = 0;
        xstring numstr = "12321";
        bool flag = numstr.ToInt(&integ);
        if(flag)
          printf("19: xstring object converted to a int number %d.\n", integ);
    }

    // 20. bool ToDouble (double *out_val) const
    {
        double real;
        xstring numstr = "-12.34";
        bool flag = numstr.ToDouble(&real);
        if(flag)
          printf("20: xstring object converted to a real number %f.\n", real);
    }

    // 21. xstring &operator= (cStringT str);
    {
        cStringT cstr = "Multibyte String";
        xstring xstr = cstr;
        printf("21: %s\n", (const char*)xstr);
    }

    // 22. xstring &operator= (cWStringT  str);
    {
        cWStringT  cstr = L"Wide string";
        xstring xstr = cstr;
        printf("22: %s\n", (const char*)xstr);
    }

    // 23. xstring &operator= (const xstring &str);
    {
        xstring temp = "Multibyte String";
        xstring xstr = temp;
        printf("23: %s\n", (const char*)xstr);
    }

    // 24. xstring &operator= (const xstring_c &str);
    {
        xstring_c xstrc = string.getxstring_c();
        xstring xstr = xstrc;
        printf("24: %s\n", (const char*)xstr);
    }

    // 25. xstring &operator= (strstream &stream); - deprecated
    {
       strstream input;
       input << "Stream" << 1 << "Input";
       xstring xstr = input;
       printf("25: %s\n", (const char*)xstr);
    }

    // 26. xstring &operator= (stringstream &stream);
    {
       stringstream input;
       input << "Stream" << 1 << "Input";
       xstring xstr = input;
       printf("26: %s\n", (const char*)xstr);
    }

    // 27. xstring &operator= (wstringstream &stream);
    {
       wstringstream input;
       input << "Stream" << 1 << "Input";
       xstring xstr = input;
       printf("27: %s\n", (const char*)xstr);
    }

    // 28. xstring &operator+= (Char ch);
    {
        xstring str = "OT";
        char ch = 'K';
        str += ch;
        printf("28: %s\n", (const char*)str);
    }

    // 29. xstring &operator+= (WChar ch);
    {
        xstring str = "OT";
        wchar_t wch = L'K';
        str += wch;
        printf("29: %s\n", (const char*)str);
    }

    // 30. xstring &operator+= (cStringT str);
    {
        xstring str = "O";
        cStringT cstr = "TK";
        str += cstr;
        printf("30: %s\n", (const char*)str);
    }

    // 31. xstring &operator+= (cWStringT str);
    {
        xstring str = "O";
        cWStringT cwstr = L"TK";
        str += cwstr;
        printf("31: %s\n", (const char*)str);
    }

    // 32. xstring &operator+= (const xstring &str);
    {
        xstring str = "OT";
        xstring str1 = "K";
        str += str1;
        printf("32: %s\n", (const char*)str);
    }

    // 33.
    // bool operator== (const xstring &str1, cStringT str2)
    // bool operator== (const cStringT str1, const xstring &str2)
    // bool operator!= (const xstring &str1, cStringT str2)
    // bool operator!= (const cStringT str1, const xstring &str2)
    {
        xstring str = "OTK";
        cStringT cstr = "OTK";
        cStringT cstr1 = "OTK ";
        (str == cstr)   ? printf("33: OK.\n"):   printf("Not OK.\n");
        (str != cstr1)  ? printf("OK.\n"):   printf("Not OK.\n");
        (cstr == str)   ? printf("OK.\n"):   printf("Not OK.\n");
        (cstr1 != str)  ? printf("OK.\n"):   printf("Not OK.\n");
    }

    // 34.
    // bool operator== (const xstring &str1, cWStringT str2)
    // bool operator== (const cWStringT str1, const xstring &str2)
    // bool operator!= (const xstring &str1, cWStringT str2)
    // bool operator!= (const cWStringT str1, const xstring &str2)
    {
        xstring str = "OTK";
        cWStringT cstr = L"OTK";
        cWStringT cstr1 = L"OTK ";
        (str == cstr)   ? printf("34: OK.\n"):   printf("Not OK.\n");
        (str != cstr1)  ? printf("OK.\n"):   printf("Not OK.\n");
        (cstr == str)   ? printf("OK.\n"):   printf("Not OK.\n");
        (cstr1 != str)  ? printf("OK.\n"):   printf("Not OK.\n");
    }

    // 35.
    // bool operator== (const xstring &str1, const xstring &str2)
    // bool operator!= (const xstring &str1, const xstring &str2)
    {
        xstring str1 = "otk";
        xstring str2 = "otk";
        xstring str3 = "OTK";
        (str1 == str2)   ? printf("35: OK.\n"):   printf("Not OK.\n");
        (str1 != str3)   ? printf("OK.\n"):   printf("Not OK.\n");
    }

    // 36.
    // xstring operator+ (const xstring &str1, cStringT str2)
    // xstring operator+ (const xstring &str1, cWStringT str2)
    {
        xstring str1 = "O";
        cStringT cstr = "T";
        cStringT cstr1 = "K";
        cWStringT cwstr = L"T";
        cWStringT cwstr1 = L"K";
        xstring str = str1 + cstr + cstr1;
        printf("36: %s.\n", (const char *)str);
        str = str1 + cwstr + cwstr1;
        printf("%s.\n", (const char *)str);
    }

    // 37.
    // xstring operator+ (cWStringT str1, const xstring &str2)
    // xstring operator+ (cStringT str1, const xstring &str2)
    {
        cStringT cstr = "O";
        xstring str = "TK";
        xstring str1 = cstr + str;
        printf("37: %s.\n", (const char *)str1);

        cWStringT cwstr = L"O";
        str1 = cwstr + str;
        printf("%s.\n", (const char *)str1);
    }

    // 38. xstring operator+ (const xstring &str1, const xstring &str2)
    {
        xstring str1 = "Object ";
        xstring str2 = "Tool";
        xstring str3 = "kit";
        xstring str = str1 + str2 + str3;
        printf("38: %s.\n", (const char *)str);
    }

    // 39. ostream &operator<< (ostream &os, const xstring &str)
    {
        xstring str = "39: OBJECT TOOLKIT C++\n";

        filebuf fb;
        fb.open ("test.txt",ios::out);
        ostream os(&fb);
        operator<<(os, str);
        fb.close();

        FILE* fp = fopen("test.txt", "r");
        FILE *stream = stdout;
        int ch = 0;
        while ((ch = getc(fp)) != EOF)
            putc(ch, stream);
        fclose(fp);

        remove("test.txt");
    }

}
