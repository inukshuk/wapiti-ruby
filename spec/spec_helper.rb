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


RSpec.configuration do |c|
	c.include(FileUtils)
end