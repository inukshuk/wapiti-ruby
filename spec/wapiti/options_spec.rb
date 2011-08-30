module Wapiti
	describe 'Options' do
		
		let(:options) { Options.new }

		it { Options.should be_an_instance_of(Class) }
		
		it { options.should_not be nil }
	
		describe '.defaults' do
			it 'returns a hash with the default options' do
				Options.defaults.keys.map(&:to_s).sort.should == Options.attribute_names.map(&:to_s)
			end
		end
		
		describe '#initialize' do
			
			it 'should fail if called with more than one parameter' do
				expect { Options.new(1,2) }.to raise_error
			end
			
			it 'should fail if called with a parameter that is no hash' do
				expect { Options.new([]) }.to raise_error
			end

			it 'should set defaults according to the supplied hash' do
				Options.new(:compact => true).should be_compact
			end
			
			it 'should accept and execute a self-yielding block' do
				opt = Options.new(:compact => true) { |o| o.sparse = true }
				opt.should be_compact
				opt.should be_sparse
			end
			
		end
		
		describe '#update' do
			it 'sets all option values according to the given hash' do
				lambda { options.update( :threads => 2 ) }.should change { options.threads }.from(1).to(2)
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
			it 'returns a large number by default' do
				options.maxiter.should > 0
			end
		end

		describe '#maxiter=' do
			it 'sets maxiter to the given value' do
				lambda { options.maxiter = 20 }.should change { options.maxiter }.to(20)
			end
		end

		describe '#stop_window' do
			it 'returns 5 by default' do
				options.stop_window.should == 5
			end
		end

		describe '#stop_window=' do
			it 'sets stop_window to the given value' do
				lambda { options.stop_window = 20 }.should change { options.stop_window }.by(15)
			end
		end

		describe '#convergence_window' do
			it 'returns 5 by default' do
				options.convergence_window.should == 5
			end
		end

		describe '#convergence_window=' do
			it 'sets convergence_window to the given value' do
				lambda { options.convergence_window = 20 }.should change { options.convergence_window }.by(15)
			end
		end


		describe '#stop_epsilon' do
			it 'returns 0.02 by default' do
				options.stop_epsilon.should == 0.02
			end
		end

		describe '#stop_epsilon=' do
			it 'sets stop_epsilon to the given value' do
				lambda { options.stop_epsilon = 0.35 }.should change { options.stop_epsilon }
			end
		end

		describe '#rho1' do
			it 'returns 0.5 by default' do
				options.rho1.should == 0.5
			end
		end

		describe '#rho1=' do
			it 'sets rho1 to the given value' do
				lambda { options.rho1 = 2.5 }.should change { options.rho1 }.by(2)
			end
		end

		describe '#rho2' do
			it 'returns 0.0001 by default' do
				options.rho2.should == 0.0001
			end
		end

		describe '#rho2=' do
			it 'sets rho2 to the given value' do
				lambda { options.rho2 = 0.0002 }.should change { options.rho2 }.by(0.0001)
			end
		end


		%w{ maxent compact sparse label check score posterior }.each do |m|
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

		%w{ pattern model algorithm devel }.each do |m|
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