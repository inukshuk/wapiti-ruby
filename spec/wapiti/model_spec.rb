module Wapiti
	describe 'Model' do
		
		describe '.train' do
			context 'given sufficient options' do
				let(:options) {
					{ 
						:pattern => File.expand_path('../../fixtures/chpattern.txt', __FILE__),
						:input => File.expand_path('../../fixtures/chtrain.txt', __FILE__)
					}
				}
				
				it 'returns a valid model instance' do
					# Model.train(options).nlbl.should == 17
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
		end
		
		describe '#label' do
			
			context 'given an empty model' do
				
			end
			
			context 'given a trained model' do
				
				let(:model) { Model.load(File.expand_path('../../fixtures/ch.mod', __FILE__)) }
				
				it 'returns an array of token-label pairs' do
					model.sync
					h = model.label(['Hello NN B-VP', ', , O', 'world NN B-NP', '! ! O'])
					h.should == []
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