try:
    import scapy.all as scapy
except ImportError:
    import scapy
from scapy.utils import PcapReader
import os
sum=0
for r,d,f in os.walk("numtxt/"):
    for file in f:
        fi = open(r+file)
        txt=fi.read()
        t1=txt[txt.rfind('lnum')+5:]
        sum+=int(t1[:t1.find(' ')])

print(sum/10)

def calint(name):
    if(os.path.exists(name)):
        for root,dir,f in os.walk(name):
            n,sum=0,0
            for file in f:
                packets = scapy.rdpcap(name+'/'+file)[-10:]
                maxseq=0
                for packet in packets:
                    if(packet['TCP'].sport==80):
                        maxseq=packet['TCP'].seq if packet['TCP'].seq>maxseq else maxseq
                sum+=maxseq
                n+=1
            print(name,' ',sum/n,'\n')
    else:
        return

for a in ['0.00003','0.00006']:
    for b in ['t7','t9','t10','t11']:
        calint('./'+a+b)
        