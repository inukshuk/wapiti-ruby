module Wapiti
	
	# Creates a model based on the given pattern and training data.
	# Returns the model's filename.
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
		
		config.output = Tempfile.new('model').path if config.output.empty?

		model = Native.train(config)

		config.output
	end
	
	module_function :train
	
end