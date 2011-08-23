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
				
				it 'should not fail' do
					expect { Native.train(options) }.not_to raise_error
				end
				
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


			%w{ input output pattern model algorithm }.each do |m|
				describe "##{m}" do
					it 'returns an empty string by default' do
						options.send(m).should be_a(String)
					end
				end

				describe "##{m}=" do
					it 'sets the input string to the given value' do
						lambda { options.send("#{m}=", 'foo') }.should change { options.send(m) }.to('foo')
					end
				end
			end
						
		end
		
	end
end