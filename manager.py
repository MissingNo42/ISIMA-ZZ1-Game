import os, struct

os.chdir('brains')

while 1:
    print("Menu:\n0 - Update format\n1 - Get best & worst\n")
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
                
