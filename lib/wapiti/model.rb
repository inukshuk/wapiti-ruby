module Wapiti

  class Model

    class << self

      def train(data, options, &block)
        config = Options.new(options, &block)

        # check configuration
        # if config.pattern.empty?
        #   raise ConfigurationError, 'invalid options: no pattern specified'
        # end

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

    attr_reader :token_count, :token_errors, :sequence_count, :sequence_errors

    def pattern
      options.pattern
    end

    def pattern=(filename)
      options.pattern = filename
    end

    alias native_label label

    def label(input, opts = nil)
      options.update(opts) unless opts.nil?
      block_given? ? native_label(input, &Proc.new) : native_label(input)
    end

    alias native_train train

    def train(input, opts = nil)
      options.update(opts) unless opts.nil?
      block_given? ? native_train(input, &Proc.new) : native_train(input)
    end


    def statistics
      s = {}
      s[:tokens] = {
        :total => token_count, :errors => @token_errors,
        :rate => token_errors.to_f / token_count.to_f * 100.0
      }
      s[:sequences] = {
        :total => sequence_count, :errors => sequence_errors,
        :rate => sequence_errors.to_f / sequence_count.to_f * 100.0
      }
      s
    end

    alias stats statistics

    def clear_counters
      @token_count = @token_errors = @sequence_count = @sequence_errors = 0
    end

    alias clear clear_counters

    # alias native_save save

    private :native_label, :native_train

  end

end
