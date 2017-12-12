source 'https://rubygems.org'
gemspec

group :debug do
  gem 'byebug', :require => false
end

group :coverage do
  gem 'simplecov', :require => false
  gem 'coveralls', :require => false if ENV['CI']
end

group :development do
  gem 'rake'
  gem 'rake-compiler'
  gem 'rspec'
end

group :extra do
  gem 'pry'
end
