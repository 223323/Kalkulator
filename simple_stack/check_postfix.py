ls=\
'''
65F246F1B0E8420F
0CFF338CB2F7FBC6
0EBFDFDCFDCFF833
00B9F25CE5FF3EB2
F3A3FFD7FB9FAAFD
5174EDAFDDE7BBDB
3D7FF7717FF9FDF2
C19FFA52D3FBEB42
E0F30CF6EFCF56D7
CF3FFFA0601FFF
'''

import sys
cs = "0123456789ABCDEF"
ls = ls.replace('\n','')
with open("postfix.c") as f:
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

