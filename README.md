vdb
===
Vanilla Database

Build instructions
==================

```
$ make
$ ./vdb-test
Opening new empty database sample db
Adding and then re-getting 10000 64-byte values
getting 10000 64-byte values
Iterator test 
All tests OK!
```

Supported API's
===============

* VDB_open : Opens up a database file

* VDB_close : Closes database file

* VDB_get : `int VDB_get(VDB *db, const void *key, void *buf);`

* VDB_put : `int VDB_put(VDB *db, const void *key, const void *value);`

* VDB_Iterator_next : `int VDB_Iterator_next(VDB_Iterator *dbi, void *kbuf, void *vbuf);`
