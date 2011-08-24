module Wapiti
  describe 'Native Extension Module' do
    
    it 'should define the wapiti version string' do
      Native::VERSION.should match /^[\d\.]+$/
    end
    
		it { Native.should respond_to(:train) }
		it { Native.should respond_to(:label) }
		it { Native.should respond_to(:dump) }
	
		describe '.train' do
			
			let(:options) { Options.new }

			it 'should require an argument' do
				expect { Native.train }.to raise_error(::ArgumentError)
			end

			it 'should raise an error if argument is no options instance' do
				expect { Native.train(0) }.to raise_error(NativeError)
			end
			
			it 'should raise an error if options not set to training mode' do
				expect { Native.train(options) }.to raise_error(NativeError)
			end
			
			context 'when given a valid configuration instance' do
				
				let(:model) { Tempfile.new('model') }
				
				before(:each) do
					options.mode = 0
					options.pattern = File.expand_path('../../fixtures/chpattern.txt', __FILE__)
					options.input = File.expand_path('../../fixtures/chtrain.txt', __FILE__)
					options.output = model.path
				end
				
				after(:each) { model.close; model.unlink }
				
				it 'should not fail' do
					expect { Native.train(options) }.not_to raise_error
				end
				
			end
			
		end
		
		describe '.label' do
		end
		
		describe '.dump' do
		end
		
	
	end
end