ls=\
'''
k13773cqbgyy0qdf
jbc3464ztnc9i2cs
te4eq7everpupe6m
eesi7mecrw3greze
rpe2hp99ee03nm77
xxnexe7rvp3zpsee
ezt2leuceze3herq
i86laors2ber1dej
epfrwpvepoeiepoe
zq3ece1bbtxrhs2b
h2cz2eefyobue1n2
bekikeif2e4cepfk
e2e0uw2jieke6i8m
qea6n76wp4chgobm
emp53bmeehee3e8b
memcekeaveq4eeqy
b7214y0bzej6ek0b
zeb6egp4e3ef6wye
ker6x2nkieiyce5x
ncceso0cbe3ehrja
n7ezxa8rxee6le7w
e6b4nm7fteeevq4h
4eee
'''

import sys
def lr(a,b): return list(range(ord(a),ord(b)+1))
cs = ''.join(chr(i) for i in (lr('0','9') + lr('a','z')))
ls = ls.replace('\n','')
with open(sys.argv[1] if len(sys.argv) > 1 else "with_stack_functions.c") as f:
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
	if line < len(ls): printf('missing', len(ls) - line, 'lines')

