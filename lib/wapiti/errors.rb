module Wapiti
	
	class Error < StandardError
		
		attr_accessor :original
		
		def initialize(message = '', original = $!)
			super(message)
			@original = original
		end
		
	end

	class NativeError < Error; end
	class ConfigurationError < Error; end
	
end