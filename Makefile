OBJS = \
	hash/hash.o \
	hash/hash_tofile.o \
	hash/hash_tostr.o \
	hash/memhash.o \
	hash/u32hash.o \
	lines/data.o \
	lines/lines.o \
	utils/base64.o \
	utils/crc32c.o \
	utils/webencode.o \

CFLAGS = -O3 -Wall
CPPFLAGS = -I.

libbksupport.a: $(OBJS)
	$(AR) r $@ $^
	ranlib $@

depend:
	makedepend $(CPPFLAGS) $(OBJS:%.o=%.c)

clean:
	rm -f $(OBJS) libbksupport.a

# DO NOT DELETE

hash/hash.o: hash/hash.h /usr/include/errno.h /usr/include/features.h
hash/hash.o: /usr/include/stdc-predef.h /usr/include/assert.h
hash/hash.o: /usr/include/stdio.h /usr/include/string.h
hash/hash.o: /usr/include/strings.h /usr/include/stdlib.h
hash/hash.o: /usr/include/alloca.h style.h hash/memhash.h hash/u32hash.h
hash/hash_tofile.o: hash/hash.h /usr/include/errno.h /usr/include/features.h
hash/hash_tofile.o: /usr/include/stdc-predef.h /usr/include/assert.h
hash/hash_tofile.o: /usr/include/stdio.h /usr/include/string.h
hash/hash_tofile.o: /usr/include/strings.h /usr/include/stdlib.h
hash/hash_tofile.o: /usr/include/alloca.h style.h /usr/include/ctype.h
hash/hash_tofile.o: /usr/include/endian.h lines/lines.h utils/base64.h
hash/hash_tofile.o: utils/webencode.h
hash/hash_tostr.o: hash/hash.h /usr/include/errno.h /usr/include/features.h
hash/hash_tostr.o: /usr/include/stdc-predef.h /usr/include/assert.h
hash/hash_tostr.o: /usr/include/stdio.h /usr/include/string.h
hash/hash_tostr.o: /usr/include/strings.h /usr/include/stdlib.h
hash/hash_tostr.o: /usr/include/alloca.h style.h utils/webencode.h
hash/memhash.o: hash/hash.h /usr/include/errno.h /usr/include/features.h
hash/memhash.o: /usr/include/stdc-predef.h /usr/include/assert.h
hash/memhash.o: /usr/include/stdio.h /usr/include/string.h
hash/memhash.o: /usr/include/strings.h /usr/include/stdlib.h
hash/memhash.o: /usr/include/alloca.h style.h hash/memhash.h utils/crc32c.h
hash/u32hash.o: hash/hash.h /usr/include/errno.h /usr/include/features.h
hash/u32hash.o: /usr/include/stdc-predef.h /usr/include/assert.h
hash/u32hash.o: /usr/include/stdio.h /usr/include/string.h
hash/u32hash.o: /usr/include/strings.h /usr/include/stdlib.h
hash/u32hash.o: /usr/include/alloca.h style.h hash/u32hash.h lines/data.h
lines/data.o: style.h lines/data.h /usr/include/assert.h
lines/data.o: /usr/include/features.h /usr/include/stdc-predef.h
lines/data.o: /usr/include/stdlib.h /usr/include/alloca.h
lines/data.o: /usr/include/string.h /usr/include/strings.h
lines/lines.o: style.h lines/lines.h /usr/include/assert.h
lines/lines.o: /usr/include/features.h /usr/include/stdc-predef.h
lines/lines.o: /usr/include/stdlib.h /usr/include/alloca.h
lines/lines.o: /usr/include/string.h /usr/include/strings.h
lines/lines.o: /usr/include/stdio.h /usr/include/unistd.h
utils/base64.o: utils/base64.h /usr/include/stdlib.h /usr/include/alloca.h
utils/base64.o: /usr/include/features.h /usr/include/stdc-predef.h
utils/base64.o: /usr/include/string.h /usr/include/strings.h
utils/crc32c.o: utils/crc32c.h /usr/include/stdlib.h /usr/include/alloca.h
utils/crc32c.o: /usr/include/features.h /usr/include/stdc-predef.h style.h
utils/webencode.o: utils/webencode.h /usr/include/stdio.h style.h
utils/webencode.o: /usr/include/assert.h /usr/include/features.h
utils/webencode.o: /usr/include/stdc-predef.h /usr/include/stdlib.h
utils/webencode.o: /usr/include/alloca.h /usr/include/string.h
utils/webencode.o: /usr/include/strings.h
