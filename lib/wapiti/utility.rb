module Wapiti
	
	# Creates a model based on the given pattern and training data.
	# Returns the model's filename.
	def train(options = {}, &block)
		config = Options.new(options, &block)
		
		raise ArgumentError, 'no training data specified as input option' if config.input.empty?
		raise ArgumentError, 'no pattern specified in options' if config.pattern.empty?
		
		config.mode = 0
		config.output = Tempfile.new('model').path if config.output.empty?
		
		Native.train(config)
		
		config.output
	end
	
	module_function :train
	
end