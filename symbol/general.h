/******************************************************************************
 *  CVS version:
 *     $Id: general.h,v 1.1 2004/05/05 22:00:08 nickie Exp $
 ******************************************************************************
 *
 *  C header file : general.h
 *  Project       : PCL Compiler
 *  Version       : 1.0 alpha
 *  Written by    : Nikolaos S. Papaspyrou (nickie@softlab.ntua.gr)
 *  Date          : May 5, 2004
 *  Description   : Generic symbol table in C, general header file
 *
 *  Comments: (in Greek iso-8859-7)
 *  ---------
 *  Εθνικό Μετσόβιο Πολυτεχνείο.
 *  Σχολή Ηλεκτρολόγων Μηχανικών και Μηχανικών Υπολογιστών.
 *  Τομέας Τεχνολογίας Πληροφορικής και Υπολογιστών.
 *  Εργαστήριο Τεχνολογίας Λογισμικού
 */


#ifndef __GENERAL_H__
#define __GENERAL_H__


/* ---------------------------------------------------------------------
 * ----------- Πρωτότυπα των συναρτήσεων διαχείρισης μνήμης ------------
 * --------------------------------------------------------------------- */

// just some variantes of malloc and free, with error handling
// don't have to check for null return value 
void * new    (size_t);
void   delete (void *);


/* ---------------------------------------------------------------------
   -------------- Καθολικές μεταβλητές του μεταγλωττιστή ---------------
   --------------------------------------------------------------------- */

// gets automatically updated by lexer, should put extern 
extern int linecount;
// should be updated by our main program , these are just for error reporting
extern const char * filename;


#endif
