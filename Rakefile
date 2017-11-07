require 'bundler'
begin
  Bundler.setup
rescue Bundler::BundlerError => e
  $stderr.puts e.message
  $stderr.puts "Run `bundle install` to install missing gems"
  exit e.status_code
end

require 'rake/clean'
require 'rake/extensiontask'

$:.unshift(File.join(File.dirname(__FILE__), './lib'))
require 'wapiti/version'


Rake::ExtensionTask.new do |ext|
  ext.name = 'native'

  ext.ext_dir = 'ext/wapiti'
  ext.lib_dir = 'lib/wapiti'

  CLEAN.include("#{ext.lib_dir}/native.*")
  CLEAN.include("#{ext.tmp_dir}")
end

require 'rspec/core'
require 'rspec/core/rake_task'
RSpec::Core::RakeTask.new(:spec) do |spec|
  spec.pattern = FileList['spec/**/*_spec.rb']
end

desc 'Run an IRB session with Wapiti loaded'
task :console do
  require 'pry'
  require 'wapiti'

  Pry.start
end

task :check_warnings do
  $VERBOSE = true
  require 'wapiti'
  puts Wapiti::VERSION
end

begin
  require 'coveralls/rake/task'
  Coveralls::RakeTask.new
  task :test_with_coveralls => [:compile, :spec, 'coveralls:push']
rescue LoadError
  # ignore
end

task :default => [:compile, :spec]

task :build => [:clean] do
  system 'gem build wapiti.gemspec'
end

task :release => [:build] do
  system "git tag #{Wapiti::VERSION}"
  system "gem push wapiti-#{Wapiti::VERSION}.gem"
end

CLEAN.include('*.gem')
CLEAN.include('*.rbc')
