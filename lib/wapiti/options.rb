module Wapiti
  class Options
    include Comparable

    @attribute_names = %w{
      algorithm
      check
      compact
      compress
      convergence_window
      jobsize
      max_iterations
      maxent
      pattern
      posterior
      rho1
      rho2
      score
      skip_tokens
      sparse
      stop_epsilon
      stop_window
      threads
      type
    }.map(&:to_sym).freeze

    @algorithms = %w{
      auto
      bcd
      l-bfgs
      rprop
      rprop+
      rprop-
      sgd-l1
    }.freeze

    @types = %{
      crf
      maxent
      memm
    }.freeze

    class << self
      attr_reader :attribute_names, :algorithms, :types

      # Returns the default options.
      def defaults
        @defaults ||= new.attributes
      end
    end

    attr_accessor :compress

    alias compress? compress

    # Returns the value of the attribute identified by +name+ or nil
    # if there is no such attribute.
    def [](name)
      has_attribute?(name) ? send(name) : nil
    end

    # Updates the value of the attribute identified by +name+ with the
    # passed-in +value+.
    def []=(name, value)
      raise ArgumentError,
        "bad attribute name: #{name}" unless has_attribute?(name)

      send("#{name}=", value)
    end

    # Updates all the attributes from the passed-in hash.
    def update(attributes = {})
      attributes.each_pair do |k, v|
        mid = "#{k}="
        send(mid, v) if respond_to?(mid)
      end
      self
    end

    alias update_attributes update

    def update!(*args)
      update(*args)
      validate!
    end

    def lbfgs
      attributes(:clip, :histsz, :maxls)
    end

    def sgdl1
      attributes(:eta0, :alpha)
    end

    def bcd
      attributes(:kappa)
    end

    def rprop
      attributes(:stpmin, :stpmax, :stpinc, :stpdec, :cutoff)
    end

    # Returns a hash of the given attributes with their names and values.
    def attributes(attrs = Options.attribute_names)
      Hash[*attrs.map { |a| [a, send(a)] }.flatten]
    end

    alias to_hash attributes

    def has_attribute?(attribute)
      Options.attribute_names.include?(attribute)
    end

    def valid_algorithm?
      self.class.algorithms.include?(algorithm)
    end

    def valid_type?
      self.class.types.include?(type)
    end

    def valid?
      validate.empty?
    end


    def validate
      e = []

      %w{ threads jobsize alpha histsz maxls eta0 alpha nbest }.each do |name|
        e << "invalid value for #{name}: #{send(name)}" unless send(name) > 0
      end

      %w{ rho1 rho2 }.each do |name|
        e << "invalid value for #{name}: #{send(name)}" unless send(name) >= 0.0
      end

      e << "unknown type: #{type}" unless valid_type?
      e << "unknown algorithm: #{algorithm}" unless valid_algorithm?
      e << "BCD not supported for training maxent models" if maxent && algorithm == 'bcd'
      e
    end

    %w{ maxent compact sparse label check score posterior compress }.each do |m|
      writer = "#{m}=".to_sym
      define_method("#{m}!") do
        send(writer, true)
        self
      end
    end

    def validate!
      errors = validate
      raise ArgumentError, errors.join('; ') unless errors.empty?
    end

    def <=>(other)
      other.respond_to?(:attributes) ? attributes <=> other.attributes : nil
    end
  end
end
