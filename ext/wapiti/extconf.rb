require 'mkmf'

$CFLAGS << %q{ -std=c99 -W -Wall -Wextra -O3 }

have_library('pthread')
have_library('m')

create_makefile('wapiti/native')
