module Wapiti
  describe Sequence do
    describe '.parse' do
      it 'parses a sequence of tokens' do
        seq = Sequence.parse(<<~EOS, tagged: true)
          Confidence NN B-NP
          in IN B-PP
          the DT B-NP
          pound NN I-NP
          is VBZ B-VP
          widely RB I-VP
          expected VBN I-VP
          to TO I-VP
          take VB I-VP
          another DT B-NP
          sharp JJ I-NP
          dive NN I-NP
          . . O
        EOS

        expect(seq.size).to eq(13)
        expect(seq[-2].to_s(expanded: false)).to eq('dive I-NP')
        expect(seq[1].observations).to eq(['IN'])
      end
    end

    let(:s) {
      Sequence.new([
        Token.new('Confidence', label: 'B-NP', observations: ['NN']),
        Token.new('in', label: 'B-PP', observations: ['IN']),
        Token.new('the', label: 'B-NP', observations: ['DT']),
        Token.new('pound', label: 'I-NP', observations: ['NN']),
        Token.new('is', label: 'B-VP', observations: ['VBZ']),
        Token.new('widely', label: 'I-VP', observations: ['RB']),
        Token.new('expected', label: 'I-VP', observations: ['VBN']),
        Token.new('to', label: 'I-VP', observations: ['TO']),
        Token.new('take', label: 'I-VP', observations: ['VB']),
        Token.new('another', label: 'B-NP', observations: ['DT']),
        Token.new('sharp', label: 'I-NP', observations: ['JJ']),
        Token.new('dive', label: 'I-NP', observations: ['NN']),
        Token.new('.', label: 'O', observations: ['.']),
      ])
    }

    describe '<=>' do
    end

    describe '#to_s' do
      it 'returns expanded tokens by default' do
        expect(s.to_s).to eq(<<~EOS.strip)
          Confidence NN B-NP
          in IN B-PP
          the DT B-NP
          pound NN I-NP
          is VBZ B-VP
          widely RB I-VP
          expected VBN I-VP
          to TO I-VP
          take VB I-VP
          another DT B-NP
          sharp JJ I-NP
          dive NN I-NP
          . . O
        EOS
      end
    end

    describe '#to_a' do
      it 'returns an array of token strings' do
        expect(s.to_a.length).to eq(13)
        expect(s.to_a.first).to eq('Confidence NN B-NP')
        expect(s.to_a(tagged: false).first).to eq('Confidence NN')
      end
    end

    describe '#to_h' do
      it 'returns a hash of segment arrays' do
        expect(s.to_h.keys).to eq(%w{B-NP B-PP I-NP B-VP I-VP O})
        expect(s.to_h['I-VP']).to eq(['widely expected to take'])
        expect(s.to_h['I-NP']).to eq(['pound', 'sharp dive'])
      end
    end

    describe '#to_xml' do
      it 'returns a <sequence> node' do
        expect(s.to_xml(Builder::XmlMarkup.new(indent: 2)).to_s).to eq(<<~EOS)
          <sequence>
            <B-NP>Confidence</B-NP>
            <B-PP>in</B-PP>
            <B-NP>the</B-NP>
            <I-NP>pound</I-NP>
            <B-VP>is</B-VP>
            <I-VP>widely expected to take</I-VP>
            <B-NP>another</B-NP>
            <I-NP>sharp dive</I-NP>
            <O>.</O>
          </sequence>
        EOS
      end
    end
  end
end
