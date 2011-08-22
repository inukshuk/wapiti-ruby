# -*- encoding: utf-8 -*-
lib = File.expand_path('../lib/', __FILE__)
$:.unshift lib unless $:.include?(lib)

require 'wapiti/version'

Gem::Specification.new do |s|
  s.name        = 'crfpp'
  s.version     = Wapiti::VERSION.dup
  s.platform    = Gem::Platform::RUBY
  s.authors     = ['Sylvester Keil']
  s.email       = ['http://sylvester.keil.or.at']
  s.homepage    = 'https://github.com/inukshuk/wapiti'
  s.summary     = 'Conditional Random Fields for Ruby.'
  s.description = 'Conditional Random Fields as a Wapiti-powered Ruby extension.'
  s.license     = 'FreeBSD'

  s.add_development_dependency('rake', '~>0.9')
  s.add_development_dependency('rake-compiler', '~>0.7')
  s.add_development_dependency('ZenTest', '~>4.6')
  s.add_development_dependency('rspec', '~>2.6')  

  s.files        = `git ls-files`.split("\n")
  s.test_files   = `git ls-files -- {test,spec,features}/*`.split("\n")
  s.executables  = []
  s.require_path = 'lib'
  
  s.extensions << 'ext/crfpp/extconf.rb'

  s.rdoc_options      = %w{--line-numbers --inline-source --title "Wapiti" --main README.md --webcvs=http://github.com/inukshuk/crfpp/tree/master/}
  s.extra_rdoc_files  = %w{README.md}
  
end

# vim: syntax=ruby