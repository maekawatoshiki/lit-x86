lib File
	proto File_open name:string mode:string | file_open
	proto File_write id str:string | file_write
	proto File_read id:int : string | file_read
	proto File_close id | file_close
end

def file_open name:string
	file_open name "r"
end


