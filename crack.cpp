#include "crack.h"


//constructor
crackzip::crackzip()
{
}


//helper function to read file
uint32_t crackzip::fget_32 (FILE *f)
{
    uint32_t r;
    r  = fgetc(f) <<  0;
    r |= fgetc(f) <<  8;
    r |= fgetc(f) << 16;
    r |= fgetc(f) << 24;
    return r;
}

uint32_t crackzip::fget_16 (FILE *f)
{
    uint32_t r;
    r  = fgetc(f) <<  0;
    r |= fgetc(f) <<  8;
    return r;
}


//constructor
crackzip::crackzip(string filename, string dict_file)
{
    //load zip files and read dictionary
    this->filename=filename;
    loadzip(filename);
    open_dict(dict_file);
}

//open dictionary
void crackzip::open_dict(string dict_file)
{
    dict_f.open(dict_file,ifstream::in);
}


//load zip file, find crc value
void crackzip::loadzip(string filename)
{
    //open file
    FILE *f = fopen (filename.c_str(), "rb");
    if (!f)
	{
	    cout <<"file doesn't exist."<<endl;
	    return;
	}
    
    //read file info
    while (!feof (f))
	{
	    //check file ID
	    id = fget_32(f);
	    if (id == ZIPID)
		{
		    //read basic info
		    version = fget_16(f);
		    flags = fget_16(f);
		    compression_method = fget_16(f);
		    lastmodtime = fget_16(f);
		    lastmoddate = fget_16(f);
		    crc32 = fget_32(f);
		    compr_size = fget_32(f);
		    uncompr_size = fget_32(f);
		    name_len = fget_16(f);
		    extra_field_len = fget_16(f);
		    
		    //skip file name field
		    fread (zip_name, name_len, 1, f);
		    cout <<"file name:"<<zip_name<<endl;
		    cout <<"flag:"<<flags<<endl;
		    //skip extra field
		    fseek (f, extra_field_len, SEEK_CUR);
		    
		    //read 12 bytes to decrypt
		    fread (file, FILE_SIZE, 1, f);

		    //check if it contains data descriptor
		    if (flags & 0x8)
			truecrc=lastmodtime;
		    else
			truecrc=crc32;
		    cout <<"CRC32:"<<hex<<truecrc<<endl;
		}
	}
}

//crack zip's password
void crackzip::crackpw()
{
    uint32_t key0, key1, key2;
    cout <<"start to crack!"<<endl;
    //record time
    clock_t start_time=clock();
    while (get_next_pw())
	{
	    //init key chain
	    key0=K0;
	    key1=K1;
	    key2=K2;
	    //cout <<"password:"<<pw<<endl;

	    //generate key
	    for (int i=0;i<pw.size();i++)
		{
		    key0 = crc32(key0, pw[i]);
		    key1 = (key1 + (uint8_t)key0) * 134775813 + 1;
		    key2 = crc32(key2, key1 >> 24);
		}
	    
	    uint8_t target[15];
	    uint16_t t;
	    //decrpyt first 12 bytes 
	    for (int i=0;i<12;i++)
		{
		    t = key2 | 2;
		    target[i] = file[i] ^ (uint8_t)(((uint16_t) (t * (t^1)) >> 8));
		    key0 = crc32 (key0, target[i]);
		    key1 = (key1 + (uint8_t)key0) * 134775813 + 1;
		    key2 = crc32 (key2, key1 >> 24);
		}
	    
	    //cout <<"target crc32:"<<hex<<(uint32_t)target[11]<<hex<<(uint32_t)target[10]<<endl;
	    //cout <<"true crc32:"<<hex<<(uint32_t)file[12]<<hex<<(uint32_t)file[13]<<endl;
	    
	    //check if CRC match
	    if (((target[11]<<8)|target[10])==truecrc)
		{
		    cout <<"possible password found:"<<pw<<endl;
		    cout <<"check password."<<endl;
		    //verify password
		    if (check_pw())
			{
			    cout <<"True Password:"<<pw<<endl;
			    cout <<"Duration:"<<(clock() - start_time) / (double) CLOCKS_PER_SEC<<endl;
			    return;
			}
		    else
			cout <<"Password check failed."<<endl;
		}
	}
    cout <<"Can't find password."<<endl;
    cout <<"Duration:"<<(clock() - start_time) / (double) CLOCKS_PER_SEC;
}

//get next password in dictionary
bool crackzip::get_next_pw()
{
    return static_cast<bool>(getline(dict_f,pw));
}

//use unzip to check if password is correct
bool crackzip::check_pw()
{
    string cmd="unzip -qqt -P "+pw+" "+filename;
    int status=system(cmd.c_str());
    if (status!=0)
	return false;
    return true;
}

//main function
int main(int argc, char *argv[])
{
    string filename=string(argv[1]);
    string dict_file=string(argv[2]);
    crackzip crack(filename,dict_file);
    crack.crackpw();
    return 0;
}
