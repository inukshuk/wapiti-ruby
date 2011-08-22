module Wapiti
  describe 'Native Extension Module' do
    
    it 'should define the wapiti version string' do
      Native::VERSION.should match /^[\d\.]+$/
    end
    
		it { Native.should respond_to(:train) }

		it { Native.should respond_to(:label) }

		it { Native.should respond_to(:dump) }
	
		describe '.train' do
			
			let(:options) { Native::Options.new }

			it 'should require an argument' do
				expect { Native.train }.to raise_error(ArgumentError)
			end

			it 'should raise an error if argument is no options instance' do
				expect { Native.train(0) }.to raise_error(NativeError)
			end
			
			it 'should raise an error if options not set to training mode' do
				expect { Native.train(options) }.not_to raise_error
			end
			
		end
		
		describe 'Options' do
			
			let(:options) { Native::Options.new }

			it { Native::Options.should be_an_instance_of(Class) }
			
			it { options.should_not be nil }
		
			describe '#mode' do
				it 'returns -1 by default' do
					options.mode.should == -1
				end
			end

			describe '#mode=' do
				it 'sets the mode to the given value' do
					lambda { options.mode = 1 }.should change { options.mode }.from(-1).to(1)
				end
			end


			describe '#input' do
				it 'returns an empty string by default' do
					options.input.should == ''
				end
			end

			describe '#input=' do
				it 'sets the input string to the given value' do
					lambda { options.input = 'foo' }.should change { options.input }.to('foo')
				end
			end

			describe '#output' do
				it 'returns an empty string by default' do
					options.output.should == ''
				end
			end

			describe '#output=' do
				it 'sets the output string to the given value' do
					lambda { options.output = 'foo' }.should change { options.output }.to('foo')
				end
			end
			
		end
		
	end
end