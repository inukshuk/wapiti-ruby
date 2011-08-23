require 'wapiti'

require 'fileutils'
require 'tempfile'

RSpec::Matchers.define :be_valid_model_file do
	match do |path|
		File.exists?(path) && !File.open(path).read.empty?
	end	
end

RSpec.configuration do |c|
	c.include(FileUtils)
end