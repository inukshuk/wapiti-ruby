
require 'logger'
require 'tempfile'

require 'wapiti/version'

module Wapiti
	
	Logger = ::Logger.new(STDOUT)
	Logger.level = ::Logger::WARN
	
	def self.log() Logger; end
	
end

require 'wapiti/errors'
require 'wapiti/native'

require 'wapiti/options'
require 'wapiti/model'

require 'wapiti/utility'