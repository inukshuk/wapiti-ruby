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

			describe '#threads' do
				it 'returns 1 by default' do
					options.threads.should == 1
				end
			end

			describe '#threads=' do
				it 'sets threads to the given value' do
					lambda { options.threads = 2 }.should change { options.threads }.from(1).to(2)
				end
			end

			describe '#jobsize' do
				it 'returns 64 by default' do
					options.jobsize.should == 64
				end
			end

			describe '#jobsize=' do
				it 'sets jobsize to the given value' do
					lambda { options.jobsize = 128 }.should change { options.jobsize }.by(64)
				end
			end

			describe '#maxiter' do
				it 'returns 0 by default' do
					options.maxiter.should == 0
				end
			end

			describe '#maxiter=' do
				it 'sets maxiter to the given value' do
					lambda { options.maxiter = 20 }.should change { options.maxiter }.by(20)
				end
			end


			%w{ maxent compact sparse }.each do |m|
				describe "##{m}" do
					it 'returns false by default' do
						options.send(m).should be false
					end
				end

				describe "##{m}=" do
					it "sets #{m} to the given value" do
						lambda { options.send("#{m}=", true) }.should change { options.send(m) }.from(false).to(true)
						lambda { options.send("#{m}=", false) }.should change { options.send(m) }.from(true).to(false)
						lambda { options.send("#{m}=", 123) }.should change { options.send(m) }.from(false).to(true)
						lambda { options.send("#{m}=", nil) }.should change { options.send(m) }.from(true).to(false)
					end
				end
			end

			%w{ input output pattern model algorithm devel }.each do |m|
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