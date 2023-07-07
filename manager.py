import os, struct
from os.path import isfile

os.chdir('brains')

def rename(a,b):
    print("rename",a,"to", b)
    os.rename(a,b)

while 1:
    print("Menu:\n0 - Update format\n1 - Get best & worst\n2 - Merge Glouton\n")
    c = input("Select : ")

    if c == '0':
        s = input("Select species ( = R, G, B) :")
        a = input("Select algo ( = 1, 2, G) :")
        ext = {'R':"red", 'G':"green", 'B':"blue"}[s]
        prf = {'1':"g1", '2':"g2", 'G':"ag"}[a]
        
        for i in os.listdir():
            if i.startswith(('0')) and i.endswith(ext):
                print("rename", i, "to", prf + '.' + i)
                os.rename(i, prf + '.' + i)

    elif c == '1':
        for s in ["red", "green", "blue"]:
            print(f"\n{s.title()}\n")
            for a in ["g1", "g2", "ag"]:
                M = float("-inf")
                Mf = "?" * 14
                m = float("inf")
                mf = "?" * 14
                for i in os.listdir():
                    if i.startswith(a) and i.endswith(s):
                        q = struct.unpack('f', open(i, 'rb').read()[-4:])[0]
                        if q >= M:
                            M = q
                            Mf = i
                        if q <= m:
                            m = q
                            mf = i
                print(f"\t{a}: Best = {Mf}\t ({M:.3f})\t\t Worst = {mf}\t ({m:.3f})")
                if '.' in Mf:
                    n = Mf.split('.')
                    n[1] = 'best'
                    r = open(Mf, 'rb')
                    dt = r.read()
                    r.close()
                    w = open('.'.join(n), 'wb')
                    w.write(dt)
                    w.close()
    elif c == '2':
        for i in os.listdir():
            if i.startswith("g1"):
                u = i.replace("g1", "g2")
                if isfile(u):
                    q = struct.unpack('f', open(i, 'rb').read()[-4:])[0]
                    p = struct.unpack('f', open(u, 'rb').read()[-4:])[0]
                    if q > p:
                        os.remove(u)
                        rename(i, i.replace("g1", "gl"))
                    else:
                        os.remove(i)
                        rename(u, u.replace("g2", "gl"))
                else: rename(i, i.replace("g1", "gl"))
            elif i.startswith("g2"):
                u = i.replace("g2", "g1")
                if isfile(u):
                    q = struct.unpack('f', open(i, 'rb').read()[-4:])[0]
                    p = struct.unpack('f', open(u, 'rb').read()[-4:])[0]
                    if q > p:
                        os.remove(u)
                        rename(i, i.replace("g2", "gl"))
                    else:
                        os.remove(i)
                        rename(u, u.replace("g1", "gl"))
                else: rename(i, i.replace("g2", "gl"))
        
                
