lib File
	proto File_open(name:string mode:string)| file_open
	proto File_write(id str:string) | file_write
	proto File_read(id):string | file_read
	proto File_close(id) | file_close
	proto File_dir_list(dir:string):string
end

struct FILE
	id
	name:string
	use?
end

module File
	def new:FILE
		new ! FILE
	end
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
		id = open name
		if id == 0
			return "error: no such file or directory"
		end
		content = read id
		file_close id
		content
	end
	def dir_list_one(dir:string):string
		File_dir_list dir
	end
	def dir_list(dir:string):string[]
		list = []:string
		while true
			dir = File::dir_list_one "."
			if dir == nil
				break
			end
			list += dir
		end
		list
	end
	def close id
		file_close id
	end
end


def open(ref file:FILE, name:string, mode:string):FILE
	if file.use?
		file.close()
	end
	file.id = File::open name, mode
	file.name = name
	file.use? = true
	file
end
def open(ref file:FILE, name:string):FILE
	open(file, name, "r")
end
def read(ref file:FILE):string
	File::read file.id
end
def write(ref file:FILE, content:string)
	File::write file.id, content
end
def close(ref file:FILE)
	File::close file.id
	file.use? = false
end
