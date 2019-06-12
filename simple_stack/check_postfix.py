ls=\
'''
k137k00nlqnt3yil
audekamuj2ezer6x
inljeefdefgnect3
fkfxsrbhreegobue
1nbek7zeifes2ena
b5pkw8a64p10ue4p
7zexztbzeekei5xn
cceso0kbe3ej7a8t
xple7we6b4nm7fte
eevq4h4eee
'''

import sys
def lr(a,b): return list(range(ord(a),ord(b)+1))
cs = ''.join(chr(i) for i in (lr('0','9') + lr('a','z')))
ls = ls.replace('\n','')
with open(sys.argv[1] if len(sys.argv) > 1 else "postfix.c") as f:
	vals=[]
	line = 0
	for i,l in enumerate(f.readlines()):
		l2 = l.replace(' ','').replace('\t','')[:-1]
		b=l2.find('//')
		if b != -1: l2=l2[:b]
		if l2.strip() != '':
			a = 0
			for j in l2: a += a*3 + ord(j)
			a = a ^ (a>>4) ^ (a>>8)
			c = cs[a % 36]
			if(line < len(ls) and c != ls[line]):
				print('line', i+1, 'error:', l[:-1])
				break
			line+=1
	if line < len(ls): print('missing', len(ls) - line, 'lines')

