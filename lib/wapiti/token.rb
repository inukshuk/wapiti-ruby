module Wapiti
  class Token
    include Comparable

    attr_accessor :value, :label, :observations

    class << self
      def parse(string, spacer: /\s+/, tagged: false)
        value, *observations = string.split(spacer)
        new(value, {
          label: (tagged ? observations.pop : nil).to_s,
          observations: observations
        })
      end
    end

    def initialize(value = '', label: '', observations: [])
      @value, @label, @observations = value, label, observations
    end

    def observations?
      !(observations.nil? || observations.empty?)
    end

    def label?
      !(label.nil? || label.empty?)
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

    def to_a(expanded: false, tagged: false)
      a = [value]

      if expanded
        raise Error,
          'cannot expand token: missing observations' unless observations?
        a.concat observations
      end

      if tagged
        raise Error,
          'cannot tag token: missing label' unless label?
        a << label
      end

      a
    end
  end
end
