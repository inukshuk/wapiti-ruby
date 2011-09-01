require 'mkmf'

$CFLAGS << %q{ -std=c99 -O3 -Wall -ggdb }

have_library('pthread')
have_library('m')

create_makefile('wapiti/native')