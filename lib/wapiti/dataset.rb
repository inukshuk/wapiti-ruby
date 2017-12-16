module Wapiti
  require 'builder'

  class Dataset
    extend Forwardable
    include Comparable
    include Enumerable

    attr_reader :sequences
    def_delegators :sequences, :[], :sample, :size, :slice!

    class << self
      def parse(
        input,
        format: format_for(input),
        separator: /(?:\r?\n){2,}/,
        **options
      )
        case format.downcase
        when 'wapiti'
          new(input.map { |seq|
            Sequence.new(seq.map { |tk|
              Token.new tk[0], label: tk[1].to_s, score: tk[2]
            })
          })
        when '.txt', 'txt'
          new(input.split(separator).map { |seq|
            Sequence.parse(seq, **options)
          })
        when '.xml', 'xml'
          new
        else
          raise ArgumentError, "unknown format: '#{format}'"
        end
      end

      def open(path, format: File.extname(path), **options)
        raise ArgumentError,
          "cannot open dataset from tainted path: '#{path}'" if path.tainted?

        parse(File.read(path, encoding: 'utf-8'), format: format, **options)
      end

      private

      def format_for(input)
        case input
        when Array
          'wapiti'
        else
          'txt'
        end
      end
    end

    def initialize(sequences = [])
      @sequences = sequences
    end

    def each
      if block_given?
        sequences.each(&Proc.new)
        self
      else
        to_enum
      end
    end

    def <=>(other)
      Dataset === other ? sequences <=> other.sequences : nil
    end

    def to_s(separator: "\n\n", **options)
      map { |sq| sq.to_s(**options) }.join(separator)
    end

    def to_a(**options)
      map { |sq| sq.to_a(**options) }
    end

    def to_xml(**options)
      xml = Builder::XmlMarkup.new(**options)
      xml.instruct!
      xml.dataset do |ds|
        each do |seq|
          seq.to_xml ds
        end
      end
    end

    def to_yml(**options)
      map { |sq| sq.to_h(**options) }
    end

    def save(path, format: File.extname(path), **options)
      raise ArgumentError,
        "cannot write dataset to tainted path: '#{path}'" if path.tainted?

      case format.downcase
      when '.txt', 'txt'
        File.write(path, to_s(**options), encoding: 'utf-8')
      when '.xml', 'xml'
        File.write(path, to_xml(**options), encoding: 'utf-8')
      else
        raise ArgumentError, "unknown format: '#{format}'"
      end
    end

    def inspect
      "#<Wapiti::Dataset sequences={#{size}}>"
    end
  end
end
