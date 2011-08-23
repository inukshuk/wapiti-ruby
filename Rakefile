lib = File.expand_path('../lib/', __FILE__)
$:.unshift lib unless $:.include?(lib)

require 'rake/clean'
require 'rake/testtask'
require 'rake/extensiontask'

require 'wapiti/version'

task :default => [:test]

Rake::ExtensionTask.new do |ext|
  ext.name = 'native'
  
  ext.ext_dir = 'ext/wapiti'
  ext.lib_dir = 'lib/wapiti'
  
  CLEAN.include("#{ext.lib_dir}/native.*")
  CLEAN.include("#{ext.tmp_dir}")  
  
end

task :build => [:clean] do
  system 'gem build wapiti.gemspec'
end

task :release => [:build] do
  system "git tag #{Wapiti::VERSION}"
  system "gem push wapiti-#{Wapiti::VERSION}.gem"
end

CLEAN.include('*.gem')
CLEAN.include('*.rbc')
