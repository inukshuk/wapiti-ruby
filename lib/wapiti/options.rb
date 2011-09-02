module Wapiti
	
	class Options
		
		include Comparable
		
		class << self
				
			# Returns a sorted list of available option attributes.
			def attribute_names
				@attribute_names ||= %w{ stop_window convergence_window posterior
					max_iterations jobsize threads rho1 rho2 stop_epsilon score check
					algorithm pattern development_data maxent compact sparse label
					}.sort.map(&:to_sym).freeze
			end
			
			# Returns the default options.
			def defaults
				@defaults ||= new.attributes
			end

			# Returns the list of supported algorithm options.
			def algorithms
				@algorithms ||= %w{ l-bfgs sgd-l1 bcd rprop rprop+ rprop- auto }.freeze
			end
			
		end
		
		# Returns the value of the attribute identified by +name+ or nil
		# if there is no such attribute.
		def [](name)
			has_attribute?(name) ? send(name) : nil
		end
		
		# Updates the value of the attribute identified by +name+ with the
		# passed-in +value+.
		def []=(name, value)
			raise ArgumentError, "bad attribute name: #{name}" unless has_attribute?(name)
			send("#{name}=", value)
		end
		
		# Updates all the attributes from the passed-in hash.
		def update(attributes = {})
			attributes.each_pair do |k,v|
				mid = "#{k}="
				send(mid, v) if respond_to?(mid)
			end	
			self
		end
		
		alias update_attributes update
		
		def lbfgs
			{ :clip => clip, :histsz => histsz, :maxls => maxls }
		end
		
		def sgdl1
			{ :eta0 => eta0, :alpha => alpha }
		end
		
		def bcd
			{ :kappa => kappa }
		end
		
		def rprop
			{
				:stpmin => stpmin, :stpmax => stpmax, :stpinc => stpinc,
				:stpdec => stpdec, :cutoff => cutoff
			}
		end
		
		# Returns a hash of all the attributes with their names and values.
		def attributes
			Hash[*Options.attribute_names.map { |a| [a, send(a)] }.flatten]
		end
		
		alias to_hash attributes
		
		def has_attribute?(attribute)
			Options.attribute_names.include?(attribute)
		end
		
		def valid_algorithm?
			self.class.algorithms.include?(algorithm)
		end
		
		def valid?
			validate.empty?
		end
		
		def validate
			e = []

			%w{ threads jobsize alpha histsz maxls eta0 alpha nbest }.each do |name|
				e << "invalid value for #{name}: #{send(name)}" unless send(name) > 0
			end

			%w{ rho1 rho2 }.each do |name|
				e << "invalid value for #{name}: #{send(name)}" unless send(name) >= 0.0
			end
			
			e << "unknown algorithm: #{algorithm}" unless valid_algorithm?			
			e << "BCD not supported for training maxent models" if maxent && algorithm == 'bcd'
			e
		end
		
		%w{ maxent compact sparse label check score posterior }.each do |m|
			writer = "#{m}=".to_sym
			define_method("#{m}!") do
				send(writer, true)
				self
			end
		end
		
		def <=>(other)
			other.respond_to?(:attributes) ? attributes <=> other.attributes : nil
		end
				
	end
	
end