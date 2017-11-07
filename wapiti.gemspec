# -*- encoding: utf-8 -*-
lib = File.expand_path('../lib/', __FILE__)
$:.unshift lib unless $:.include?(lib)

require 'wapiti/version'

Gem::Specification.new do |s|
  s.name        = 'wapiti'
  s.version     = Wapiti::VERSION.dup
  s.platform    = Gem::Platform::RUBY

  s.authors     = ['Sylvester Keil']
  s.email       = ['http://sylvester.keil.or.at']

  s.homepage    = 'https://github.com/inukshuk/wapiti-ruby'
  s.summary     = 'Wicked fast Conditional Random Fields for Ruby.'
  s.description =
    """
    This gem provides a Ruby API for Conditional Random Fields (CRF).
    """

  s.license     = 'BSD-2-Clause'
  s.date        = Time.now.strftime('%Y-%m-%d')

  s.files        = `git ls-files`.split("\n") - %w{
    vendor/wapiti
    .coveralls.yml
    .travis.yml
    .gitmodules
    .gitignore
  }

  s.test_files   = `git ls-files -- {test,spec,features}/*`.split("\n")

  s.executables  = []
  s.require_path = 'lib'

  s.extensions << 'ext/wapiti/extconf.rb'

  s.rdoc_options      = %w{--line-numbers --inline-source --title "Wapiti-Ruby" --main README.md --webcvs=http://github.com/inukshuk/wapiti-ruby/tree/master/}
  s.extra_rdoc_files  = %w{README.md LICENSE}

end

# vim: syntax=ruby
