module Wapiti
  describe 'Model' do
    let(:pattern) { fixture 'pattern.txt' }
    let(:training_data) { fixture 'train.txt' }

    describe '.train' do
      context 'given sufficient options' do
        it 'returns a valid model instance' do
          expect(
            Model.train(training_data, :pattern => pattern).labels
          ).to eq((1..6).map(&:to_s))
        end

        it 'is also exposed as Wapiti.train' do
          expect(
            Wapiti.train(training_data, :pattern => pattern).labels
          ).to eq((1..6).map(&:to_s))
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
        it 'creates the options from the hash' do
          expect(Model.new(threads: 4).options.threads).to eq(4)
        end
      end

      context 'when passed an options instance' do
        let(:options) { Options.new(threads: 4) }

        it 'should create the options from the hash' do
          expect(Model.new(options).options[:threads]).to eq(4)
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
          expect(Model.new(threads: 4) { |o| o.threads = 3 }.options.threads).to eq(3)
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
      let(:model) { Model.new(:pattern => pattern) }

      it 'accepts a filename as input' do
        expect(model.train(training_data).nlbl).to eq(6)
      end

      it 'supports different algorithm' do
        expect(model.train(training_data, nil, algorithm: 'sgd-l1').nlbl).to eq(6)
      end

      it 'supports multi-threading' do
        expect(model.train(training_data, nil, threads: 2).nlbl).to eq(6)
      end

      it 'accepts a data array' do
        data = []
        seq = []

        File.open(training_data) do |f|
          f.each_line do |line|
            if line.strip.empty?
              unless seq.empty?
                data << seq
                seq = []
              end
            else
              seq << line
            end
          end
        end

        expect(model.train(data).nlbl).to eq(6)
      end

      context 'when called without a pattern' do
        it 'fails because of wapiti' do
          expect {
            expect(Model.new.train(training_data).nlbl).to eq(6)
          }.to raise_error(NativeError)
        end
      end

    end

    describe '#statistics' do
      context 'given an empty model' do
        it 'returns zeroes' do
          expect(Model.new.stats).to eq({
            token: { count: 0, errors: 0, rate: 0 },
            sequence: { count: 0, errors: 0, rate: 0 }
          })
        end
      end

      context 'given a trained model' do
        let(:model) { Wapiti.load(fixture('ch.mod')) }
        let(:input) { [['Héllo NN B-VP', ', , O', 'world NN B-NP', '! ! O']] }

        it 'returns token and sequcence counts and errors' do
          expect(model.token_count).to eq(0)
          model.label input
          expect(model.token_count).to eq(0)

          model.label input, :check => true
          expect(model.token_count).to eq(input.map(&:length).reduce(&:+))
          expect(model.sequence_count).to eq(input.length)
          expect(model.options.check).to be(false)
        end
      end
    end

    describe '#label' do
      context 'given an empty model' do
      end

      context 'given a trained model' do
        let(:model) { Wapiti.load(fixture('ch.mod')) }

        context 'when passed an array of arrays' do
          let(:input) { [['Hello NN B-VP', ', , O', 'world NN B-NP', '! ! O']] }

          it 'returns a tagged dataset' do
            dataset = model.label(input)
            expect(dataset[0].map(&:value)).to eq(%w{ Hello , world ! })
            expect(dataset[0].map(&:label)).to eq(%w{ B-NP O B-NP O })
          end

          it 'yields each token/label pair to the supplied block' do
            dataset = model.label(input) do |token, label|
              [token.downcase, label.downcase]
            end

            expect(dataset[0].map(&:label)).to eq(%w{ b-np o b-np o })
          end

          context 'with the :score option set' do
            before(:each) { model.options.score! }

            it 'returns an array of token-label-score tuples' do
              expect(model.label(input)[0].all?(&:score?)).to be(true)
            end
          end

          context 'with the :nbest option set to 2' do
            before(:each) { model.options.nbest = 2 }

            it 'returns an array of token-label-label tuples' do
              expect(model.label(input)[0][-1][1,2]).to eq(%w{ O O })
            end
          end
        end

        context 'when passed a filename' do
          let(:input) { fixture('chtest.txt') }

          it 'returns a tagged dataset' do
            dataset = model.label(input)
            expect(dataset.size).to eq(77)
            expect(dataset[0].take(5).map(&:label)).to eq(%w{ B-NP B-PP B-NP I-NP B-VP })
          end
        end
      end
    end

    describe '#labels' do
      it 'returns an empty list by default' do
        expect(Model.new.labels).to be_empty
      end

      context 'given a trained model' do
        let(:model) { Model.load(fixture('ch.mod')) }

        it 'returns a list of all known labels' do
          expect(model.labels.size).to eq(model.nlbl)
        end
      end
    end
  end
end
