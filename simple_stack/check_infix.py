ls=\
'''
4PFM6AHt9eke12tT
jQg-fJ5kfzZfLB70
Xkf5ffEgveLahfO9
fIYwfitqb2kED+l1
GlffmkRffEnjf5E5
f4RfKifsDu000Gaf
f6k-6r-A6EHaF8rf
IBfsW21+XngnKgjx
9MfWg78ofrKMff9g
jftF59ffOMfkfj+X
lbjgK-7Z471XPX7E
55fxDhF55ffyBD5f
3f+n-Gp9fJ0BfjfX
RgVaIoYQ7AFfAKfc
H7EnxAYnfffCMEbj
fff
'''

import sys
def lr(a,b): return list(range(ord(a),ord(b)+1))
cs = ''.join(chr(i) for i in (lr('0','9') + lr('a','z') + lr('A','Z'))) + '+-*='
ls = ls.replace('\n','')
with open(sys.argv[1] if len(sys.argv) > 1 else "infix.c") as f:
	vals=[]
	line = 0
	for i,l in enumerate(f.readlines()):
		l2 = l.lstrip().replace('\t','')
		if l2.strip() != '':
			a = sum([ord(i) for i in l2])
			a = a ^ (a>>4) ^ (a>>8)
			c = cs[a & 63]
			if(line < len(ls) and c != ls[line]):
				print('line', i+1, 'error:', l[:-1])
				break
			line+=1

