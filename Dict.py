import sys
import datetime
from itertools import permutations,product


def genDict(filename):
    #first generate pwds related to input info
    #read file into a list of strings
    f = open(filename,"r")
    info = {}
    line = f.readline().strip()
    curr = ""
    #read through each line, remove the section header and put all strings of the same section into one list
    while line:
        if line.startswith('__'):
            curr = line[2:]
            info[curr] = []
        else:
            info[curr].append(line)
        line = f.readline().strip()
    f.close()
    #process each section into elements for permutations
    elems = []
    for key in info:
        #For names section, we consider upper/lower/capital case of the strings as well as initials
        if key == "names":
            perms = []
            initials = []

            for val in info[key]:
                for e in permutations(val.split(' ')):
                    perms.append(' '.join(e).lower())
            for val in perms:
                initials.append("".join([item[0] for item in val.split(' ')]))
            names = " ".join(perms) + " " +" ".join(initials)
            names1 = names.title()
            names2 = names.upper()
            
            elems.extend(names.split(' '))
            elems.extend(names1.split(' '))
            elems.extend(names2.split(' '))
        #For places section, we consider upper/lower/capital case of the strings
        if key == 'places':
            places = " ".join(info[key]).lower()
            places1 = places.title()
            places2 = places.upper()            
            elems.extend(places.split(' '))
            elems.extend(places.split(' '))
            elems.extend(places.split(' '))
        #For dates section, we take in dates in the format of MM/DD/YYYY
        #And we output different kinds of format that could be used in a password
        if key == 'dates':
            dates = []
            for val in info[key]:
                date = datetime.datetime.strptime(val,"%m/%d/%Y")
                dates.append(date.strftime('%d%m%y'))
                dates.append(date.strftime('%d%m%Y'))
                dates.append(date.strftime('%d%b%y'))
                dates.append(date.strftime('%d%b%Y'))
                dates.append(date.strftime('%y%m%d'))
                dates.append(date.strftime('%Y%m%d'))
                dates.append(date.strftime('%y%b%d'))
                dates.append(date.strftime('%Y%b%d'))
                dates.append(date.strftime('%b%d'))
                dates.append(date.strftime('%m%d'))
                dates.append(date.strftime('%d%b'))
                dates.append(date.strftime('%d%m'))
                dates.append(date.strftime('%y'))
                dates.append(date.strftime('%Y'))
            elems.extend(dates)

        #For ids, we simply add it into the list with different capitalization option
        if key == "ids":
            ids = []
            for val in info[key]:
                ids.append(val.lower())
            elems.extend(ids)
            elems.extend([item.upper() for item in ids])
            elems.extend([item.title() for item in ids])
        #For numbers, we consider the entire number, last 4 digits, and last 7 digits.
        if key == "numbers":
            elems.extend(info[key])
            elems.extend([item[-4:] for item in info[key]])
            elems.extend([item[-7:] for item in info[key]])
        #For others, same process as places
        if key == 'others':
            others = " ".join(info[key]).lower()
            others1 = others.title()
            others2 = others.upper()            
            elems.extend(others.split(' '))
            elems.extend(others.split(' '))
            elems.extend(others.split(' '))

    print "info list length:",len(elems)

    #First dictionary by permuting elements of personal informations, 2-elem and 3-elem only.
    dict1 = []
    for e in permutations(elems,2):
        dict1.append(''.join(e))

    for e in permutations(elems,3):
        dict1.append(''.join(e))
    #adding the original elems to create the first dictionary
    dict1.extend(elems)
    print "info permutation length:",len(dict1)
        
 
    #read the wordlist from project John the Ripper
    f2 = open('john.txt','r')
    common = []
    line = f2.readline()
    while line:
        common.append(line.lower())
        common.append(line.upper())
        common.append(line.title())        
        line = f2.readline()
    print "common pwds length:",len(common)

    #taking the product the this list with the personal element list to create a larger list
    dict2 = []
    for e in product(common,elems):
        dict2.append(''.join(e))
    #adding the original list to create the second dictionary
    dict2.extend(common)
    print "length of permutation of common pwds and info:",len(dict2)
    
    #combine the two dictionaries for a final one
    dict1.extend(dict2)
    #make it unique
    dict1 = list(set(dict1))
    print "total length of dictionary:",len(dict1)

    #write it into the file dictionary.txt
    f3 = open('dictionary.txt','w')
    for e in dict1:
        f3.write(e+'\n')
    f3.close()

#function call
genDict("info.txt")
