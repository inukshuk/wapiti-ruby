module Wapiti
  class Model
    class << self
      def train(training_data, options = {}, &block)
        development_data =
          options.delete(:development_data) ||
          options.delete(:data)

        config = Options.new(options, &block)
        new(config).train(training_data, development_data)
      end

      def load(filename)
        model = new
        model.path = filename
        model.load
      end
    end

    attr_accessor :path
    attr_reader :token_count, :token_errors, :sequence_count, :sequence_errors

    def pattern
      options.pattern
    end

    def pattern=(filename)
      options.pattern = filename
    end

    alias native_label label

    def label(input, opts = nil, &block)
      unless opts.nil?
        original_options = options.attributes(opts.keys)
        options.update!(opts)
      end

      input = input.to_a(tagged: options.check) if input.is_a?(Dataset)

      if block_given?
        output = native_label(input, &block)
      else
        output = native_label(input)
      end

      return output if options.nbest > 1 || options.skip_tokens

      Dataset.parse output, tagged: true
    ensure
      unless original_options.nil?
        options.update(original_options)
      end
    end

    def check(input)
      reset
      label input, check: true
      stats
    end

    alias native_train train

    def train(tdat, ddat = nil, opts = nil, &block)
      options.update!(opts) unless opts.nil?

      tdat = tdat.to_a(tagged: true) if tdat.is_a?(Dataset)
      ddat = ddat.to_a(tagged: true) if ddat.is_a?(Dataset)

      if block_given?
        native_train(tdat, ddat, &block)
      else
        native_train(tdat, ddat)
      end
    end

    def statistics
      {
        token: {
          count: token_count,
          errors: token_errors,
          rate: token_error_rate
        },
        sequence: {
          count: sequence_count,
          errors: sequence_errors,
          rate: sequence_error_rate
        }
      }
    end

    alias stats statistics

    def reset_counters
      @token_count = 0
      @token_errors = 0
      @sequence_count = 0
      @sequence_errors = 0
      self
    end

    alias reset reset_counters

    def token_error_rate
      return 0 if token_errors.zero?
      token_errors / token_count.to_f * 100.0
    end

    def sequence_error_rate
      return 0 if sequence_errors.zero?
      sequence_errors / sequence_count.to_f * 100.0
    end

    private :native_label, :native_train
  end
end
