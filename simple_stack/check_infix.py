ls=\
'''
65F246F1BC0E8420
F0CFFB59FFBF338C
B2F7FBC60EBFDFD1
FDCFF83300B9F25C
E5FF3EB2F3A3FFD7
FB9FAAF765BBBAAF
F28425064A2772EF
24FB70B1D3DD50EB
2DFAD3A4FDBDFFF0
EFB159FF0DF0FD51
74EDAFDDE7BBDB3D
57FF77157FFB385F
1FDF2C19FFA5D3FB
EEB8020F30CF6EFC
F56D7CF3FFFA0601
FFF
'''

import sys
cs = "0123456789ABCDEF"
ls = ls.replace('\n','')
with open("infix.c") as f:
	vals=[]
	line = 0
	for i,l in enumerate(f.readlines()):
		l2 = l.replace('\t','').replace(' ', '')
		if l2.strip() != '':
			a = sum([ord(i) for i in l2])
			a = a ^ (a>>4) ^ (a>>8)
			c = cs[a & 15]
			if(line < len(ls) and c != ls[line]):
				print('line', i+1, 'error:', l)
			line+=1

