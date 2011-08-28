module Wapiti
	
	class Model
		
		class << self
			
			def train(options = {}, &block)
				config = Options.new(options, &block)
				config.mode = 0
				
				# check configuration	
				if config.input.empty?
					raise ConfigurationError, 'no training data specified in options'
				end
				
				if config.pattern.empty?
					raise ConfigurationError, 'no pattern specified in options' 
				end
				
				unless config.valid_algorithm?
					raise ConfigurationError, "algorithm #{config.algorithm.inspect} is not supported"
				end
				
				new(config).train
			end
			
			def load(filename)
				new(:model => filename)
			end
			
		end

		def path
			options.model
		end
		
		def path=(filename)
			options.model = filename
		end
		
		def pattern
			options.pattern
		end
		
		def pattern=(filename)
			options.pattern = filename
		end
		
	end
	
end