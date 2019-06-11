import sys, textwrap
def sh(v):
	a = sum([ord(i) for i in v])
	return a ^ (a>>4) ^ (a>>8)
cs = "0123456789ABCDEF"
with open(sys.argv[1]) as f:
	vals=[]
	for i in f.readlines():
		i = i.replace('\t','').replace(' ', '')
		if i.strip() != '':
			
			vals.append(cs[sh(i) & 15])
	ins = "ls=\\\n'''\n" + '\n'.join(textwrap.wrap(''.join(vals), 16)) + "\n'''\n"

ins+=\
'''
import sys
cs = "0123456789ABCDEF"
ls = ls.replace('\\n','')
with open("%s") as f:
	vals=[]
	line = 0
	for i,l in enumerate(f.readlines()):
		l2 = l.replace('\\t','').replace(' ', '')
		if l2.strip() != '':
			a = sum([ord(i) for i in l2])
			a = a ^ (a>>4) ^ (a>>8)
			c = cs[a & 15]
			if(line < len(ls) and c != ls[line]):
				print('line', i+1, 'error:', l)
			line+=1
''' % sys.argv[1]
print(ins)
