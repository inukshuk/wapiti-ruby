module Wapiti
	
	# Creates a model based on the given pattern and training data.
	# Returns the model's filename.
	def train(options = {}, &block)
		Model.train(options, &block)
	end
	
	def label(options = {}, &block)
		config = Options.new(options, &block)
		config.label_mode!
		
		unless config.valid?
			raise ConfigurationError, "invalid options: #{ config.validate.join('; ') }"
		end

		Native.label(config)
	end
	
	module_function :train, :label
	
end