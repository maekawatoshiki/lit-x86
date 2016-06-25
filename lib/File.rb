lib File
	proto File_open name:string mode:string | file_open
	proto File_write id str:string | file_write
	proto File_read id:int : string | file_read
	proto File_close id | file_close
end

module File
	def open name:string mode:string
		file_open name mode
	end
	def open name:string
		file_open name "r"
	end
	def write id str:string
		file_write id str
	end
	def read id:int :string
		file_read id 
	end
	def read name:string :string
		read open name
	end
	def close id
		file_close id
	end
end

