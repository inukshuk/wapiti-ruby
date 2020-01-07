module Wapiti
  require 'builder'
  require 'forwardable'
  require 'rexml/document'

  class Dataset
    extend Forwardable
    include Comparable
    include Enumerable

    attr_reader :sequences
    def_delegators :sequences, :[], :empty?, :length, :size, :slice!, :uniq!

    class << self
      def parse(dataset, separator: /(?:\r?\n){2,}/, **opts)
        case dataset
        when Array
          new(dataset.map { |seq|
            Sequence.new(seq.map { |tk|
              value, *obs = tk[0].split(/\s+/)
              Token.new value, label: tk[1].to_s, observations: obs, score: tk[2]
            })
          })
        when String
          new(dataset.split(separator).map { |seq|
            Sequence.parse(seq, **opts)
          }.reject(&:empty?))
        when REXML::Document
          new(dataset.elements.to_a('dataset/sequence').map { |seq|
            Sequence.new(seq.elements.to_a.map { |sgm|
              sgm.text.strip.split(opts[:spacer] || /\s+/).map { |tk|
                Token.new tk, label: sgm.name
              }
            }.flatten)
          })
        else
          raise ArgumentError, "unknown input type: #{input.class}"
        end
      end

      def open(path, format: File.extname(path), **opts)
        raise ArgumentError,
          "cannot open dataset from tainted path: '#{path}'" if path.tainted?

        input = File.read(path, encoding: 'utf-8')
        case format.downcase
        when '.xml', 'xml'
          parse(REXML::Document.new(input), **opts)
        else
          parse(input, **opts)
        end
      end
    end

    def initialize(sequences = [])
      @sequences = sequences
    end

    def each(&block)
      if block_given?
        sequences.each(&block)
        self
      else
        to_enum
      end
    end

    def labels
      map { |sq| sq.map(&:label).uniq }.flatten.uniq.sort
    end

    def hash
      sequences.hash
    end

    def eql?(other)
      hash == other.hash
    end

    def <=>(other)
      Dataset === other ? sequences <=> other.sequences : nil
    end

    def concat(other)
      sequences.concat other.sequences
      self
    end

    def sample(n = 1, **opts)
      Dataset.new sequences.sample(n, **opts)
    end

    def slice(start, length = 1)
      if Range === start
        Dataset.new sequences.slice(start)
      else
        Dataset.new sequences.slice(start, length)
      end
    end

    def +(other)
      Dataset.new(sequences + other.sequences)
    end

    def -(other)
      Dataset.new(sequences - other.sequences)
    end

    def |(other)
      Dataset.new(sequences | other.sequences)
    end

    def &(other)
      Dataset.new(sequences & other.sequences)
    end

    def to_s(separator: "\n\n", **opts)
      map { |sq| sq.to_s(**opts) }.join(separator)
    end

    def to_txt(separator: "\n", **opts)
      map { |sq| sq.to_sentence(**opts) }.join(separator)
    end

    def to_a(**opts)
      map { |sq| sq.to_a(**opts) }
    end

    def to_xml(**opts)
      xml = Builder::XmlMarkup.new(**opts)
      xml.instruct!
      xml.dataset do |ds|
        each do |seq|
          seq.to_xml ds
        end
      end
    end

    def to_yml(**opts)
      map { |sq| sq.to_h(**opts) }
    end

    def save(path, format: File.extname(path), **opts)
      raise ArgumentError,
        "cannot write dataset to tainted path: '#{path}'" if path.tainted?

      output = case format.downcase
        when '.txt', 'txt'
          to_s(**opts)
        when '.xml', 'xml'
          to_xml(**opts)
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
