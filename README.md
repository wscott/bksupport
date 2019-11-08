# bksupport - C utility functions from BitKeeper

This is a C library of useful code that was originally written for
[BitKeeper](https://github.com/bitkeeper-scm/bitkeeper).
They provide a clear, but opionated set of utilities for some data structures
missing from C.

## hash - virtual API to hash functions

The `hash/hash.h` header provides with a virtual API for multiple map
implementations. Use `hash *h = hash_new(TYPE)` to open a new hash
table and then it includes a number of interfaces for accessing
different types of data.

## memhash - separate chaining store of arbitrary date

Memhash is the main backend for the `hash` API. It uses memory
allocation for each key/value pair that is part of a linked chain on
the main change table. All operations are supported. Pointers returned
from the hash are stable and so this hash can be used for memory
allocation.

```
  hash *h = hash_new(HASH_MEMHASH);
  char *v

  hash_storeStrStr(h, "key1", "value1");
  hash_storeStrStr(h, "key2", "value2");

  EACH_HASH(h) {
    printf("%s -> %s\n", (char *)h->kptr, (char *)h->vptr);
  }
  if (hash_fetchStr(h, "key3")) {
    printf("found key3 -> %s\n", (char *)h->vptr);
  }
  hash_free(h);
```

(see hash/hash.h for detailed usage)

## u32hash - specialized compact hash for 32-bit keys

U32hash is a hash designed for small memory footprints and good cache
locality. The hash table uses open-addressing with linear reprobing
and puts the keys directly in the hash table for fast lookup. The data
items are stored contiguously in a single data array in the order the
ideas were added to the hash. Items cannot be deleted from this
hash. The data locations in this hash are not stable and should can
change after every insertion.

## data

Like `std::vector<char>` for C. This is a dynamcally growing data
buffer where data can be appended.

```
  DATA file = {0};

  while (fgets(buf, sizeof(buf), f)) {
    if (buf[0] == '#') continue;  // skip comments
    data_appendStr(&file, buf);
  }
  processFile(file.buf);
  free(file.buf);
```

(see lines/data.h for details)

## lines

This library contains two sets of APIs for dealing with dynamically
sized arrays of data.

The `xxxLines()` functions are for dealing with an array of
pointers. Usually C strings, but that is not required.

```
  char **lines = 0;
  int i;

  lines = addLines(lines, strdup("foo"));
  lines = addLines(lines, strdup("bar"));
  sortLines(lines);
  EACH(lines) {
    printf("%s\n", lines[i]);
  }
  char **f = splitLine("a;b;c;d;e", ";", 0);
  removeLine(f, findLine(f, "c"), free);
```

The `xxxArray()` functions are for dealing with a dynamic packed array
of arbitrary types.  (generic programming in C)

```
  struct foo *list = 0;
  while (1) {
      struct foo x;
	  fetchNew(&x);
      addArray(&list, &x);
  }
  int i;
  for (i = 1; i <= nLines(list); i++) {
      process(&list[i]);
  }
  free(list);
```
(see lines/lines.h for details)

## util

Misc functions. At the moment only a couple are include:
* base64_encode(), base64_decode()
* webencode(), webdecode()
  Encodes and decodes web URLs. Including splitting arguments into
  lines arrays.
* crc32c
  A fast crc32c library.
