import sys, textwrap
def sh(v):
	a = 0
	for i in v: a += a*3 + ord(i)
	return a ^ (a>>4) ^ (a>>8)
def lr(a,b): return list(range(ord(a),ord(b)+1))
cs = ''.join(chr(i) for i in (lr('0','9') + lr('a','z')))
with open(sys.argv[1]) as f:
	vals=[]
	for i in f.readlines():
		i = i.replace(' ','').replace('\t','')[:-1]
		a = i.find('//')
		if a != -1:
			i=i[:a]
		if i.strip() != '':
			vals.append(cs[sh(i) % 36])
	ins = "ls=\\\n'''\n" + '\n'.join(textwrap.wrap(''.join(vals), 16)) + "\n'''\n"

ins+=\
'''
import sys
def lr(a,b): return list(range(ord(a),ord(b)+1))
cs = ''.join(chr(i) for i in (lr('0','9') + lr('a','z')))
ls = ls.replace('\\n','')
with open(sys.argv[1] if len(sys.argv) > 1 else "%s") as f:
	vals=[]
	line = 0
	for i,l in enumerate(f.readlines()):
		l2 = l.replace(' ','').replace('\\t','')[:-1]
		b=l2.find('//')
		if b != -1: l2=l2[:b]
		if l2.strip() != '':
			a = 0
			for j in l2: a += a*3 + ord(j)
			a = a ^ (a>>4) ^ (a>>8)
			c = cs[a %% 36]
			if(line < len(ls) and c != ls[line]):
				print('line', i+1, 'error:', l[:-1])
				break
			line+=1
	if line < len(ls): print('missing', len(ls) - line, 'lines')
''' % sys.argv[1]
print(ins)
