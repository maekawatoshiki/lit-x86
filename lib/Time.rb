struct TIME
	tm_sec
	tm_min	
	tm_hour
	tm_mday
	tm_mon	
	tm_year
	tm_wday
	tm_yday
	tm_isdst
end

lib Time
proto Time_time:TIME | _lib_time_time
proto Time_sleep s:double | _lib_time_sleep
end

module Time
	def new:TIME
		_lib_time_time()
	end
	def time:TIME
		_lib_time_time()
	end
	def sleep sec
		_lib_time_sleep(<double>sec)
	end
	def sleep sec:double
		_lib_time_sleep(sec)
	end
end

def get(ref t:TIME):TIME
	t = Time::time()
end
def to_string(t:TIME):string
	"{}-{}-{} {}:{}:{}" % (1900+t.tm_year) % (1+t.tm_mon) % t.tm_mday % t.tm_hour % t.tm_min % t.tm_sec
end
