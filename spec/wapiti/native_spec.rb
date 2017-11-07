module Wapiti
  describe 'Native Extension Module' do

    it 'should define the wapiti version string' do
      expect(Native::VERSION).to match(/^[\d\.]+$/)
    end

    it { expect(Native).to respond_to(:label) }

  end
end
