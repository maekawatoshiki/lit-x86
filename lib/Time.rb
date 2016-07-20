lib Time
	proto Time_time | _lib_time_time
	proto Time_strtime | _lib_time_strtime
	proto Time_sleep s:double | _lib_time_sleep
end

module Time
	def time
		_lib_time_time
	end
	def to_str:string
		_lib_time_strtime
	end
	def sleep sec:double
		_lib_time_sleep sec
	end
end


