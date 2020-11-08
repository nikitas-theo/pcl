/******************************************************************************
 *  CVS version:
 *     $Id: error.h,v 1.1 2003/05/13 22:21:01 nickie Exp $
 ******************************************************************************
 *
 *  C header file : error.h
 *  Project       : PCL Compiler
 *  Version       : 1.0 alpha
 *  Written by    : Nikolaos S. Papaspyrou (nickie@softlab.ntua.gr)
 *  Date          : May 14, 2003
 *  Description   : Generic symbol table in C, simple error handler
 *
 *  Comments: (in Greek iso-8859-7)
 *  ---------
 *  ������ �������� �����������.
 *  ����� ������������ ��������� ��� ��������� �����������.
 *  ������ ����������� ������������ ��� �����������.
 *  ���������� ����������� ����������
 */


#ifdef __cplusplus
extern "C" {
#endif

#ifndef __ERROR_H__
#define __ERROR_H__


/* ---------------------------------------------------------------------
   --------- ��������� ��� ����������� ��� �������� ��������� ----------
   --------------------------------------------------------------------- */
// can use this error handler, instead of yylex? 
void internal (const char * fmt, ...);
void fatal    (const char * fmt, ...);
void error    (const char * fmt, ...);
void warning  (const char * fmt, ...);


#endif


#ifdef __cplusplus
}
#endif

/* #endif of the include guard here */
