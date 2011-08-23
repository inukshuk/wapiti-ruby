
require 'wapiti/version'

require 'logger'

module Wapiti
	
	Logger = ::Logger.new(STDOUT)
	Logger.level = ::Logger::WARN
	
	def self.log() Logger; end
	
end

require 'wapiti/errors'
require 'wapiti/native'