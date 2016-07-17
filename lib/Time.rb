lib Time
	proto Time_time | _lib_time_time
	proto Time_strtime | _lib_time_strtime
end

module Time
	def time
		_lib_time_time
	end
	def to_str:string
		_lib_time_strtime
	end
end


