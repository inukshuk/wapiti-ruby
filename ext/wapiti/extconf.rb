require 'mkmf'

$CFLAGS << ' ' << %w{
  -DWAPITI_RUBY
  -std=c99
  -W
  -Wall
  -Wno-declaration-after-statement
  -O3
}.join(' ')

have_library('pthread')
have_library('m')

create_makefile('wapiti/native')
