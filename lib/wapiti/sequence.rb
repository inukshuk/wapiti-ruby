module Wapiti
  require 'builder'
  require 'forwardable'

  class Sequence
    extend Forwardable
    include Comparable
    include Enumerable

    attr_reader :tokens
    def_delegators :tokens, :[], :empty?, :size

    class << self
      def parse(string, delimiter: /\r?\n/, **opts)
        new(string.split(delimiter).map { |token|
          Token.parse token, **opts
        }.reject(&:empty?))
      end
    end

    def initialize(tokens = [])
      @tokens = tokens
    end

    def hash
      tokens.hash
    end

    def eql?(other)
      hash == other.hash
    end

    def <=>(other)
      Sequence === other ? tokens <=> other.tokens : nil
    end

    def tagged?
      tokens.all?(&:label?)
    end

    def token_length
      tokens.reduce(0) { |len, tk| len + tk.value.length }
    end

    def label?(label)
      tokens.any? { |tk| tk.label == label }
    end

    def labels
      tokens.map(&:label).uniq
    end

    def each
      if block_given?
        tokens.each(&Proc.new)
        self
      else
        to_enum
      end
    end

    alias_method :each_token, :each

    def each_segment(spacer: ' ')
      if block_given?
        lbl, sgm = nil, []

        each do |tk|
          if lbl != tk.label
            yield [lbl, sgm.join(spacer)] unless sgm.empty?
            lbl, sgm = tk.label, [tk.value]
          else
            sgm << tk.value
          end
        end

        yield [lbl, sgm.join(spacer)] unless sgm.empty?

        self
      else
        to_enum :each_segment
      end
    end

    def to_a(**opts)
      tokens.map { |tk| tk.to_s(**opts) }
    end

    def to_s(delimiter: "\n", **opts)
      tokens.map { |tk| tk.to_s(**opts) }.join(delimiter)
    end

    def to_sentence(delimiter: ' ')
      to_s(delimiter: delimiter, expanded: false, tagged: false)
    end

    def to_h(symbolize_keys: false, **opts)
      each_segment(**opts).reduce({}) do |h, (label, segment)|
        label = label.intern if symbolize_keys
        h[label] = [] unless h.key? label
        h[label] << segment
        h
      end
    end

    def to_xml(xml = Builder::XmlMarkup.new)
      xml.sequence do |sq|
        each_segment do |(label, segment)|
          sq.tag! label, segment
        end
      end
    end

    def inspect
      "#<Wapiti::Sequence tokens={#{size}}>"
    end
  end
end
