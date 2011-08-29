module Wapiti
	
	class Model
		
		class << self
			
			def train(options = {}, &block)
				config = Options.new(options, &block)
				config.training_mode!
				
				# check configuration					
				%w{ pattern input }.each do |data|
					if config.send(data).empty?
						raise ConfigurationError, "invalid options: no #{data} data specified"
					end
				end
				
				unless config.valid?
					raise ConfigurationError, "invalid options: #{ config.validate.join('; ') }"
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
		
		def tokenize(input)
			input
		end
		
	end
	
end