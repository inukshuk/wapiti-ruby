require 'mkmf'
require 'rbconfig'

cflags = %w{
  -std=c99
  -W
  -Wall
  -Wno-declaration-after-statement
  -O3
}

case RbConfig::CONFIG['host_os']
when /^linux/i
  cflags[0] = '-std=gnu99'
end

$CFLAGS << ' ' << cflags.join(' ')

have_library('pthread')
have_library('m')

create_makefile('wapiti/native')
