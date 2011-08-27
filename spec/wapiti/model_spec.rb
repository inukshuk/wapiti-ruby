module Wapiti
	describe 'Model' do
		
		describe 'initialization' do
		
			context 'when passed no arguments' do
				it 'creates a new model with default options' do
					m = Model.new
					m.should_not be nil
					m.options.should be_instance_of(Options)
				end
			end
			
			context 'when passed more than one argument' do
				it 'should raise an error' do
					expect { Model.new(1,2) }.to raise_error
				end
			end

			context 'when passed a hash' do
				let(:options) { { :threads => 42 } }
				
				it 'should create the options from the hash' do
					Model.new(options).options[:threads].should == 42
				end
			end
			
			context 'when passed an options instance' do
				let(:options) { Options.new(:threads => 42) }

				it 'should create the options from the hash' do
					Model.new(options).options[:threads].should == 42
				end
			end
			
			context 'when passed something other than a hash or an options instance' do
				it 'should raise an error' do
					expect { Model.new(1) }.to raise_error
					expect { Model.new(nil) }.to raise_error
					expect { Model.new(true) }.to raise_error
					expect { Model.new('foo') }.to raise_error
				end
			end
			
			context 'when called with a block' do
				it 'should pass the options instance to the block' do
					Model.new(:threads => 42) { |o| o.threads = 23 }.options.threads.should == 23
				end
			end
			
		end
		
	end
end