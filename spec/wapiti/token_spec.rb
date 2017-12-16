module Wapiti
  describe 'Token' do
    describe '.parse' do
      it 'parses single values' do
        expect(Token.parse('September').value).to eq('September')
      end

      it 'parses additional values as observations' do
        token = Token.parse('September NNP B-NP')
        expect(token.value).to eq('September')
        expect(token.observations).to eq(['NNP', 'B-NP'])
        expect(token.label).to be_empty
      end

      it 'parses labels when tagged' do
        token = Token.parse('September NNP B-NP', tagged: true)
        expect(token.value).to eq('September')
        expect(token.observations).to eq(['NNP'])
        expect(token.label).to eq('B-NP')

        token = Token.parse('September B-NP', tagged: true)
        expect(token.value).to eq('September')
        expect(token.observations).to be_empty
        expect(token.label).to eq('B-NP')
      end
    end

    describe '<=>' do
      context 'same value' do
        let(:a) { Token.new('October') }
        let(:b) { Token.new('October') }

        it 'returns -1, 0, 1 based on labels' do
          expect(a <=> b).to eq(0)
          a.label = 'B-NP'
          expect(a <=> b).to eq(1)
          expect(b <=> a).to eq(-1)
          b.label = 'B-NP'
          expect(a <=> b).to eq(0)
        end
      end

      context 'different values' do
        let(:a) { Token.new('October') }
        let(:b) { Token.new('November', label: 'B-NP') }

        it 'returns -1, 1 based on value' do
          expect(a <=> b).to eq(1)
        end
      end

      it 'returns nil when called on non-token' do
        expect(Token.new <=> '').to be_nil
      end
    end

    describe '#to_s' do
      let(:september) {
        Token.new('September', label: 'B-NP', observations: ['NNP'])
      }

      it 'returns value, observations, and label by default' do
        expect(september.to_s).to eq('September NNP B-NP')
      end

      it 'includes observations when not tagged' do
        expect(september.to_s(tagged: false)).to eq('September NNP')
      end

      it 'includes label when not expanded' do
        expect(september.to_s(expanded: false)).to eq('September B-NP')
      end

      it 'returns just the value when neither expanded not tagged' do
        expect(
          september.to_s(expanded: false, tagged: false)
        ).to eq('September')
      end
    end
  end
end
