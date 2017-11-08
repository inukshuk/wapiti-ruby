# -*- encoding: utf-8 -*-

module Wapiti
  describe 'Model' do

    describe '.train' do
      context 'given sufficient options' do
        let(:pattern) { File.expand_path('../../fixtures/pattern.txt', __FILE__) }
        let(:input) { File.expand_path('../../fixtures/train.txt', __FILE__) }

        it 'returns a valid model instance' do
          expect(Model.train(input, :pattern => pattern).labels).to eq((1..6).map(&:to_s))
        end

        it 'is also exposed as Wapiti.train' do
          expect(Wapiti.train(input, :pattern => pattern).labels).to eq((1..6).map(&:to_s))
        end

      end
    end

    describe 'initialization' do

      context 'when passed no arguments' do
        it 'creates a new model with default options' do
          m = Model.new
          expect(m).not_to be nil
          expect(m.options).to be_instance_of(Options)
          expect(m.nlbl).to eq(0)
          expect(m.nobs).to eq(0)
        end
      end

      context 'when passed more than one argument' do
        it 'should raise an error' do
          expect { Model.new(1, 2) }.to raise_error(ArgumentError)
        end
      end

      context 'when passed a hash' do
        let(:options) { { :threads => 42 } }

        it 'should create the options from the hash' do
          expect(Model.new(options).options[:threads]).to eq(42)
        end
      end

      context 'when passed an options instance' do
        let(:options) { Options.new(:threads => 42) }

        it 'should create the options from the hash' do
          expect(Model.new(options).options[:threads]).to eq(42)
        end
      end

      context 'when passed something other than a hash or an options instance' do
        it 'should raise an error' do
          expect { Model.new(1) }.to raise_error(ArgumentError)
          expect { Model.new(nil) }.to raise_error(ArgumentError)
          expect { Model.new(true) }.to raise_error(ArgumentError)
          expect { Model.new('foo') }.to raise_error(ArgumentError)
        end
      end

      context 'when called with a block' do
        it 'should pass the options instance to the block' do
          expect(Model.new(:threads => 42) { |o| o.threads = 23 }.options.threads).to eq(23)
        end
      end
    end

    describe '#options' do
      it 'returns the options for training' do
        expect(Model.new.options).to be_instance_of(Options)
      end
    end

    describe '#labels' do
      it 'returns the number of labels (0 by default)' do
        expect(Model.new.nlbl).to eq(0)
      end
    end

    describe '#observations' do
      it 'returns the number of observations (0 by default)' do
        expect(Model.new.observations).to eq(0)
      end
    end

    describe '#features' do
      it 'returns the number of features (0 by default)' do
        expect(Model.new.features).to eq(0)
      end
    end

    describe '#train' do
      let(:model) {
        Model.new(
          :pattern => File.expand_path('../../fixtures/pattern.txt', __FILE__)
        )
      }
      let(:data) { File.expand_path('../../fixtures/train.txt', __FILE__) }

      it 'accepts a filename as input' do
        expect(model.train(data).nlbl).to eq(6)
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
          expect {
            expect(Model.new.train(data).nlbl).to eq(6)
          }.to raise_error(NativeError)
        end
      end

    end

    describe '#statistics' do
      context 'given an empty model' do
        it 'returns zeroes' do
          s = Model.new.statistics

          expect(s[:sequences][:total]).to eq(0)
          expect(s[:sequences][:errors]).to eq(0)
          expect(s[:sequences][:rate]).to eq(0)
          expect(s[:tokens][:total]).to eq(0)
          expect(s[:tokens][:errors]).to eq(0)
          expect(s[:tokens][:rate]).to eq(0)
        end
      end

      context 'given a trained model' do
        let(:model) { Wapiti.load(File.expand_path('../../fixtures/ch.mod', __FILE__)) }
        let(:input) { [['Héllo NN B-VP', ', , O', 'world NN B-NP', '! ! O']] }

        it 'returns token and sequcence counts and errors' do
          expect(model.statistics[:tokens][:total]).to eq(0)
          model.label input
          expect(model.statistics[:tokens][:total]).to eq(0)

          model.options.check = true
          model.label input

          expect(model.statistics[:tokens][:total]).to eq(input.map(&:length).reduce(&:+))
          expect(model.statistics[:sequences][:total]).to eq(input.length)
        end
      end
    end

    describe '#label' do

      context 'given an empty model' do

      end

      context 'given a trained model' do
        let(:model) { Wapiti.load(File.expand_path('../../fixtures/ch.mod', __FILE__)) }

        context 'when passed an array of arrays' do
          let(:input) { [['Héllo NN B-VP', ', , O', 'world NN B-NP', '! ! O']] }

          it 'returns an array of token-label pairs' do
            labels = model.label(input)
            expect(labels[0].map(&:first)).to eq(input[0])
            expect(labels[0].map(&:last)).to eq(%w{ B-NP O B-NP O })
          end

          it 'yields each token/label pair to the supplied block' do
            labels = model.label(input) do |token, label|
              [token.downcase, label.downcase]
            end
            expect(labels[0].map(&:last)).to eq(%w{ b-np o b-np o })
          end

          context 'with the :score option set' do
            before(:each) { model.options.score! }

            it 'returns an array of token-label-score tuples' do
              model.label(input)[0].map { |t,l,s| s.class }.uniq == [Float]
            end
          end

          context 'with the :nbest option set to 2' do
            before(:each) { model.options.nbest = 2 }

            it 'returns an array of token-label-label tuples' do
              model.label(input)[0][-1][1,2] == %w{ O O }
            end
          end

        end


        context 'when passed a filename' do
          let(:input) { File.expand_path('../../fixtures/chtest.txt', __FILE__) }

          it 'returns an array of token-label pairs' do
            labels = model.label(input)
            expect(labels.size).to eq(77)
            expect(labels[0].take(5).map(&:last)).to eq(%w{ B-NP B-PP B-NP I-NP B-VP })
          end
        end

      end

    end

    describe '#labels' do
      it 'returns an empty list by default' do
        expect(Model.new.labels).to be_empty
      end

      context 'given a trained model' do
        let(:model) { Model.load(File.expand_path('../../fixtures/ch.mod', __FILE__)) }

        it 'returns a list of all known labels' do
          expect(model.labels.size).to eq(model.nlbl)
        end
      end
    end


  end
end
