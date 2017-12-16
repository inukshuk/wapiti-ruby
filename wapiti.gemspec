# -*- encoding: utf-8 -*-
lib = File.expand_path('../lib/', __FILE__)
$:.unshift lib unless $:.include?(lib)

require 'wapiti/version'

Gem::Specification.new do |s|
  s.name         = 'wapiti'
  s.version      = Wapiti::VERSION.dup
  s.platform     = Gem::Platform::RUBY
  s.authors      = ['Sylvester Keil']
  s.email        = ['sylvester@keil.or.at']
  s.license      = 'BSD-2-Clause'
  s.homepage     = 'https://github.com/inukshuk/wapiti-ruby'
  s.summary      = 'Wicked fast Conditional Random Fields for Ruby.'
  s.description  = 'This gem provides a Ruby API for Conditional Random Fields (CRF).'
  s.date         = Time.now.strftime('%Y-%m-%d')
  s.require_path = 'lib'
  s.extensions   << 'ext/wapiti/extconf.rb'

  s.required_ruby_version = '>=2.3'
  s.add_runtime_dependency('builder', '~>3.2')

  s.files =
    `git ls-files`.split('\n') - `git ls-files spec/*`.split('\n') - %w{
      .coveralls.yml
      .travis.yml
      .gitmodules
      .gitignore
      .rspec
      .simplecov
      .travis.yml
      Gemfile
      Rakefile
      appveyor.yml
      ext/wapiti/wapiti.c
      wapiti.gemspec
    }

  s.rdoc_options = %w{
    --line-numbers
    --inline-source
    --title "Wapiti-Ruby"
    --main README.md
    --webcvs=https://github.com/inukshuk/wapiti-ruby/tree/master/
  }
  s.extra_rdoc_files = %w{README.md HISTORY.md LICENSE}
end

# vim: syntax=ruby
