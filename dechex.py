import sys

def Main():
    file = f'./resources/text/{sys.argv[1]}'
    outp = f'{file}OUT.tsv'
    file = f'{file}.tsv'
    uftd = []
    writ = []
    with open(file) as tran:
        for i in tran:
            if not i[0].isnumeric():
                writ.append(i[:-1:1])
                continue
            writ.append('\t'.join([i.split('\t')[0], (b''.join([chr(j).encode('utf-8') for j in [ord(k) if ord(k) != 32 else 0 for k in i.split('\t')[1]]])[:-3:1]).decode('utf-8')]))
    with open(outp, 'w') as tran: tran.write('\n'.join(writ))
if __name__ == "__main__":
    Main();
