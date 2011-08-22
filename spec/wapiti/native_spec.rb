module Wapiti
  describe 'Native Extension Module' do
    
    it 'should define the wapiti version string' do
      Native::VERSION.should match /^[\d\.]+$/
    end
    
		it { Native.should respond_to(:train) }

		it { Native.should respond_to(:label) }

		it { Native.should respond_to(:dump) }
		
	end
end