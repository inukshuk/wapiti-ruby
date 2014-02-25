require 'mkmf'

$CFLAGS << %q{ -std=c99 -W -Wall -Wno-declaration-after-statement -O3 }

have_library('pthread')
have_library('m')

create_makefile('wapiti/native')
