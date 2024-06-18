import sys

f=f'./resources/text/{sys.argv[1]}'
o=f'{f}OUT.tsv'
f=f'{f}.tsv'
with open(f) as r:
    with open(o, 'w') as w:
        w.write('\n'.join([i if not i[0].isnumeric() else '\t'.join([i.split('\t')[0], (b''.join([chr(j).encode('utf-8') for j in [ord(k) for k in i.split('\t')[1]]]))]) for i in r]))
