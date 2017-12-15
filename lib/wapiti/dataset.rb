module Wapiti
  class DataSet
    extend Forwardable
    include Comparable
    include Enumerable

    attr_reader :sequences
    def_delegators :sequences, :[], :size, :sample

    class << self
      def parse(string, format: 'txt', separator: /(?:\r?\n){2,}/, **options)
        case format.downcase
        when '.txt', 'txt'
          new(string.split(separator).map { |seq|
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
    end

    def to_s(separator: '\n\n', **options)
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
  end
end
