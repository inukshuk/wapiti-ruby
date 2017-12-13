module Wapiti
  require 'logger'

  Logger = ::Logger.new(STDERR, {
    :level => :info,
    :progname => 'wapiti',
    :formatter => proc { |level, time, name, msg|
      "#{level} [#{time}] #{name}: #{msg}"
    }
  })

  class << self
    def log
      Logger
    end

    def quiet!
      log.level = :error
    end

    def debug!
      log.level = :debug
    end
  end
end
