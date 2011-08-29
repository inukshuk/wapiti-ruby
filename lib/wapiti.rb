
require 'logger'
require 'tempfile'

require 'wapiti/version'

module Wapiti
	
	Logger = ::Logger.new(STDOUT)
	Logger.level = ::Logger::WARN
	
	class << self
		def log
			Logger
		end
		
		def debug!
			log.level == ::Logger::DEBUG
		end
	end
	
end

require 'wapiti/errors'
require 'wapiti/native'

require 'wapiti/options'
require 'wapiti/model'

require 'wapiti/utility'