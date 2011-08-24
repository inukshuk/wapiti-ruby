module Wapiti
		
	class Options
		
		def update(attributes = {})
			attributes.each_pair do |k,v|
				mid = "#{k}="
				send(mid, v) if respond_to?(mid)
			end
			
			self
		end
		
	end
	
end