source 'https://rubygems.org'
gemspec

group :debug do
  gem 'byebug', :require => false, :platform => :mri
  gem 'rubinius-compiler', '~>2.0', :require => false, :platform => :rbx
  gem 'rubinius-debugger', '~>2.0', :require => false, :platform => :rbx
end

group :coverage do
  gem 'simplecov', :require => false
  gem 'rubinius-coverage', :platform => :rbx
  gem 'coveralls', :require => false
end

group :development do
  gem 'rake'
  gem 'rake-compiler'
  gem 'rspec'
end

group :extra do
  gem 'pry'
end

platform :rbx do
  gem 'rubysl', '~>2.0'
  gem 'racc'
  gem 'json'
end
