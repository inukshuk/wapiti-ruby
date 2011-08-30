module Wapiti
	
	class Model
		
		class << self
			
			def train(data, options, &block)
				config = Options.new(options, &block)
				
				# check configuration					
				if config.pattern.empty?
					raise ConfigurationError, 'invalid options: no pattern specified'
				end
				
				unless config.valid?
					raise ConfigurationError, "invalid options: #{ config.validate.join('; ') }"
				end				
				
				new(config).train(data)
			end
			
			def load(filename)
				m = new
				m.path = filename
				m.load
				m
			end

		end

		attr_accessor :path
		
		def pattern
			options.pattern
		end
		
		def pattern=(filename)
			options.pattern = filename
		end
		
		private
		
		def tokenize(input)
			input
		end
		
	end
	
end