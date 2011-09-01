module Wapiti
	describe 'Model' do
		
		describe '.train' do
			context 'given sufficient options' do
				let(:pattern) { File.expand_path('../../fixtures/pattern.txt', __FILE__) }
				let(:input) { File.expand_path('../../fixtures/train.txt', __FILE__) }
				
				it 'returns a valid model instance' do
					Model.train(input, :pattern => pattern).labels.should == (1..6).map(&:to_s)
				end
				
			end
		end
		
		describe 'initialization' do
		
			context 'when passed no arguments' do
				it 'creates a new model with default options' do
					m = Model.new
					m.should_not be nil
					m.options.should be_instance_of(Options)
					m.nlbl.should == 0
					m.nobs.should == 0
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
		
		describe '#options' do
			it 'returns the options for training' do
				Model.new.options.should be_instance_of(Options)
			end
		end
		
		describe '#labels' do
			it 'returns the number of labels (0 by default)' do
				Model.new.nlbl.should == 0
			end
		end

		describe '#observations' do
			it 'returns the number of observations (0 by default)' do
				Model.new.observations.should == 0
			end
		end

		describe '#features' do
			it 'returns the number of features (0 by default)' do
				Model.new.features.should == 0
			end
		end

		describe '#total' do
			it 'returns the total training time (0.0 by default)' do
				Model.new.total.should == 0.0
			end
		end

		describe '#train' do
			let(:model) { Model.new(:pattern => File.expand_path('../../fixtures/pattern.txt', __FILE__)) }
			let(:data) { File.expand_path('../../fixtures/train.txt', __FILE__) }
			
			it 'accepts a filename as input' do
				model.train(data).nlbl.should == 6
			end
			
			it 'accepts a data array' do
				# sequence = []
				# File.open(data).each_line do |line|
				# 	
				# end
				# 
				# model.train([]).nlbl.should == 6
			end
			
			context 'when called without a pattern' do
				it 'fails because of wapiti' do
					expect { Model.new.train(data).nlbl.should == 6 }.to raise_error(NativeError)
				end
			end
			
		end
		
		describe '#label' do
			
			context 'given an empty model' do
				
			end
			
			context 'given a trained model' do
				let(:model) { Model.load(File.expand_path('../../fixtures/ch.mod', __FILE__)) }
				
				context 'when passed an array of arrays' do
					let(:input) { [['Hello NN B-VP', ', , O', 'world NN B-NP', '! ! O']] }
					
					it 'returns an array of token-label pairs' do
						labels = model.label(input)
						labels[0].map(&:first).should == input[0]
						labels[0].map(&:last).should == %w{ B-NP O B-NP O }
					end
					
					it 'yields each token/label pair to the supplied block' do
						labels = model.label(input) do |token, label|
				      [token.downcase, label.downcase]
				    end
						labels[0].map(&:last).should == %w{ b-np o b-np o }
				  end
				
				end

				
				context 'when passed a filename' do
					let(:input) { File.expand_path('../../fixtures/chtest.txt', __FILE__) }
					
					it 'returns an array of token-label pairs' do
						labels = model.label(input)
						labels.should have(77).elements
						labels[0].take(5).map(&:last).should == %w{ B-NP B-PP B-NP I-NP B-VP }
					end
				end
				
			end
			
		end
	
		describe '#labels' do
			it 'returns an empty list by default' do
				Model.new.labels.should be_empty
			end
			
			context 'given a trained model' do
				let(:model) { Model.load(File.expand_path('../../fixtures/ch.mod', __FILE__)) }
			
				it 'returns a list of all known labels' do
					model.labels.should have(model.nlbl).elements
				end
			end
		end
		
		
	end
end