begin
  require 'simplecov'
rescue LoadError
  # ignore
end

begin
  require 'debug'
rescue LoadError
  # ignore
end

$:.unshift(File.join(File.dirname(__FILE__), '..', 'lib'))
$:.unshift(File.dirname(__FILE__))

require 'wapiti'

require 'fileutils'
require 'tempfile'

RSpec::Matchers.define :be_valid_model do
  match do |model|
    model.is_a?(Wapiti::Model) && model.nlbl > 0
  end
end

RSpec::Matchers.define :be_valid_model_file do
  match do |path|
    File.exists?(path) && !File.open(path).read.empty?
  end
end

module Fixtures
  FIXTURES = File.expand_path('../fixtures', __FILE__)

  def fixture(name)
    File.join FIXTURES, name
  end
end

RSpec.configure do |config|
  config.include(FileUtils)
  config.include(Fixtures)
end
