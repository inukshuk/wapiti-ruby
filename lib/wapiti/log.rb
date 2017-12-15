module Wapiti
  require 'logger'

  Logger = ::Logger.new(STDERR, {
    :level => ENV['WAPITI_LOG_LEVEL'] || :error,
    :progname => 'wapiti',
    :formatter => proc { |level, time, name, msg|
      "#{level} [#{time}] #{name}: #{msg}\n"
    }
  })

  class << self
    def log
      Logger
    end

    def quiet!
      log.level = :error
    end

    def verbose!
      log.level = :info
    end

    def debug!
      log.level = :debug
    end
  end
end
