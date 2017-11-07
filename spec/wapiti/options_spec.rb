module Wapiti
  describe 'Options' do

    let(:options) { Options.new }

    it { expect(Options).to be_an_instance_of(Class) }

    it { expect(options).not_to be nil }

    describe '.defaults' do
      it 'returns a hash with the default options' do
        expect(Options.defaults.keys.map(&:to_s).sort).to eq(Options.attribute_names.map(&:to_s))
      end
    end

    describe '#initialize' do

      it 'should fail if called with more than one parameter' do
        expect { Options.new(1, 2) }.to raise_error(ArgumentError)
      end

      it 'should fail if called with a parameter that is no hash' do
        expect { Options.new([]) }.to raise_error(TypeError)
      end

      it 'should set defaults according to the supplied hash' do
        expect(Options.new(:compact => true)).to be_compact
      end

      it 'should accept and execute a self-yielding block' do
        opt = Options.new(:compact => true) { |o| o.sparse = true }
        expect(opt).to be_compact
        expect(opt).to be_sparse
      end

    end

    describe 'array accessors' do
      it 'are supported' do
        expect { options[:threads] = 2 }.to change { options[:threads] }.from(1).to(2)
      end

      it 'fail for unknown attribute names' do
        expect { options[:unknown] = 2 }.to raise_error(ArgumentError)
      end
    end

    describe '#update' do
      it 'sets all option values according to the given hash' do
        expect { options.update( :threads => 2 ) }.to change { options.threads }.from(1).to(2)
      end
    end

    describe '#threads' do
      it 'returns 1 by default' do
        expect(options.threads).to eq(1)
      end
    end

    describe '#threads=' do
      it 'sets threads to the given value' do
        expect { options.threads = 2 }.to change { options.threads }.from(1).to(2)
      end
    end

    describe '#jobsize' do
      it 'returns 64 by default' do
        expect(options.jobsize).to eq(64)
      end
    end

    describe '#jobsize=' do
      it 'sets jobsize to the given value' do
        expect { options.jobsize = 128 }.to change { options.jobsize }.by(64)
      end
    end

    describe '#maxiter' do
      it 'returns a large number by default' do
        expect(options.maxiter).to be > 0
      end
    end

    describe '#maxiter=' do
      it 'sets maxiter to the given value' do
        expect { options.maxiter = 20 }.to change { options.maxiter }.to(20)
      end
    end

    describe '#stop_window' do
      it 'returns 5 by default' do
        expect(options.stop_window).to eq(5)
      end
    end

    describe '#stop_window=' do
      it 'sets stop_window to the given value' do
        expect { options.stop_window = 20 }.to change { options.stop_window }.by(15)
      end
    end

    describe '#convergence_window' do
      it 'returns 5 by default' do
        expect(options.convergence_window).to eq(5)
      end
    end

    describe '#convergence_window=' do
      it 'sets convergence_window to the given value' do
        expect { options.convergence_window = 20 }.to change { options.convergence_window }.by(15)
      end
    end


    describe '#stop_epsilon' do
      it 'returns 0.02 by default' do
        expect(options.stop_epsilon).to eq(0.02)
      end
    end

    describe '#stop_epsilon=' do
      it 'sets stop_epsilon to the given value' do
        expect { options.stop_epsilon = 0.35 }.to change { options.stop_epsilon }
      end
    end

    describe '#rho1' do
      it 'returns 0.5 by default' do
        expect(options.rho1).to eq(0.5)
      end
    end

    describe '#rho1=' do
      it 'sets rho1 to the given value' do
        expect { options.rho1 = 2.5 }.to change { options.rho1 }.by(2)
      end
    end

    describe '#rho2' do
      it 'returns 0.0001 by default' do
        expect(options.rho2).to eq(0.0001)
      end
    end

    describe '#rho2=' do
      it 'sets rho2 to the given value' do
        expect { options.rho2 = 0.0002 }.to change { options.rho2 }.by(0.0001)
      end
    end


    %w{ maxent compact sparse skip_tokens check score posterior }.each do |m|
      describe "##{m}" do
        it 'returns false by default' do
          expect(options.send(m)).to be false
        end
      end

      describe "##{m}=" do
        it "sets #{m} to the given value" do
          expect { options.send("#{m}=", true) }.to change { options.send(m) }.from(false).to(true)
          expect { options.send("#{m}=", false) }.to change { options.send(m) }.from(true).to(false)
          expect { options.send("#{m}=", 123) }.to change { options.send(m) }.from(false).to(true)
          expect { options.send("#{m}=", nil) }.to change { options.send(m) }.from(true).to(false)
        end
      end
    end

    %w{ pattern model algorithm devel }.each do |m|
      describe "##{m}" do
        it 'returns an empty string by default' do
          expect(options.send(m)).to be_a(String)
        end
      end

      describe "##{m}=" do
        it 'sets the input string to the given value' do
          expect { options.send("#{m}=", 'foo') }.to change { options.send(m) }.to('foo')
        end
      end
    end

  end

end
