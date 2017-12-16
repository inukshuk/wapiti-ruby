module Wapiti
  require 'builder'
  require 'rexml/document'

  class Dataset
    extend Forwardable
    include Comparable
    include Enumerable

    attr_reader :sequences
    def_delegators :sequences, :[], :sample, :size, :slice!

    class << self
      def parse(dataset, separator: /(?:\r?\n){2,}/, **options)
        case dataset
        when Array
          new(dataset.map { |seq|
            Sequence.new(seq.map { |tk|
              Token.new tk[0], label: tk[1].to_s, score: tk[2]
            })
          })
        when String
          new(dataset.split(separator).map { |seq|
            Sequence.parse(seq, **options)
          })
        when REXML::Document
          new(dataset.elements.to_a('dataset/sequence').map { |seq|
            Sequence.new(seq.elements.to_a.map { |sgm|
              sgm.text.split(options[:spacer] || /\s+/).map { |tk|
                Token.new tk, label: sgm.name
              }
            }.flatten)
          })
        else
          raise ArgumentError, "unknown input type: #{input.class}"
        end
      end

      def open(path, format: File.extname(path), **options)
        raise ArgumentError,
          "cannot open dataset from tainted path: '#{path}'" if path.tainted?

        input = File.read(path, encoding: 'utf-8')
        case format.downcase
        when '.xml', 'xml'
          parse(REXML::Document.new(input), **options)
        else
          parse(input, **options)
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

    def diff(other)
      raise NotImplementedError
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

      output = case format.downcase
        when '.txt', 'txt'
          to_s(**options)
        when '.xml', 'xml'
          to_xml(**options)
        else
          raise ArgumentError, "unknown format: '#{format}'"
        end

      File.write(path, output, encoding: 'utf-8', mode: 'w')
    end

    def inspect
      "#<Wapiti::Dataset sequences={#{size}}>"
    end
  end
end
