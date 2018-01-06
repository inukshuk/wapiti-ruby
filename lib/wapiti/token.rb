module Wapiti
  class Token
    include Comparable

    attr_accessor :value, :label, :observations, :score

    class << self
      def parse(string, spacer: /\s+/, tagged: false)
        value, *observations = string.split(spacer)
        new(value, {
          label: (tagged ? observations.pop : nil).to_s,
          observations: observations
        })
      end
    end

    def initialize(value = '', label: '', observations: [], score: nil)
      @value, @label, @observations, @score = value, label, observations, score
    end

    def observations?
      !(observations.nil? || observations.empty?)
    end

    def empty?
      value.nil? || value.strip.empty?
    end

    def label?
      !(label.nil? || label.empty?)
    end

    def score?
      !score.nil?
    end

    def hash
      [value.to_s, label.to_s].hash
    end

    def eql?(other)
      hash == other.hash
    end

    def <=>(other)
      if other.is_a?(Token)
        [value.to_s, label.to_s] <=> [other.value.to_s, other.label.to_s]
      else
        nil
      end
    end

    def to_s(spacer: ' ', **options)
      to_a(**options).join(spacer)
    end

    def to_a(expanded: true, tagged: true)
      a = [value]
      a.concat observations if expanded && observations?
      a << label if tagged && label?
      a
    end

    def inspect
      %Q{#<Wapiti::Token "#{to_s tagged: true}">}
    end
  end
end
