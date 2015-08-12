# dbow

DBOW is a database compiler-compiler or front-end.  It takes table
definitions in a relatively high-level language and prepares C,
C++, PHP, Perl (etc) functions for manipulating the database.  It
will also produce SQL table data for MySQL.

DBOW allows a user to define a database table in a meta-language
and produce SQL for generating the table.  It also provides C, C++,
PHP, Perl functions or methods for manipulating the table, such as
insert/delete/search/update.  The system is used to remove the
grunt-work of writing class libraries or access functions for
database tables and having to update several different source files
if the SQL table format should change.  The resultant code provides
a language-native mechanism for manipulating the database.

Currently it supports C, C++, SQL and PHP (with Perl under way).
It will be extended to support other languages and database constructs
as required. It currently compiles/runs on FreeBSD/Linux.  It
doesn't require any other libraries or elements except YACC/Bison.

If you have any questions, queries, comments, bugs, bug fixes or
stuff related to DBOW, please send me an email at dtynan@kalopa.com.
