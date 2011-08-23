require 'spec_helper'

describe 'Utility Methods' do
	
	let(:model) { '/tmp/wapiti_test_model' }
	let(:pattern) { File.expand_path('../../fixtures/chpattern.txt', __FILE__) }
	let(:input) { File.expand_path('../../fixtures/chtrain.txt', __FILE__) }
	
	before(:each) do
		FileUtils.touch model
	end
	
	after(:each) do
		FileUtils.rm model
	end
	
	describe 'train' do
		it 'creates a new model according to the parameters given in the options hash' do
			Wapiti.train(:pattern => pattern, :input => input).should be_valid_model_file
		end

		it 'creates a new model according to the options set in the block' do
			Wapiti.train { |c|
				c.pattern = pattern
				c.input = input
			}.should be_valid_model_file
		end

	end
	
end