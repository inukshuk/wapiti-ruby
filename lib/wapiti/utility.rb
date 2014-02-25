module Wapiti

  # Creates a Model based on the pattern in the passed-in options and training
  # +data+ supplied as input.
  def train(data, options, &block)
    Model.train(data, options, &block)
  end

  def load(model)
    Model.load(model)
  end

  module_function :train, :load

end
