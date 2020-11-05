try:
    import scapy.all as scapy
except ImportError:
    import scapy
from scapy.utils import PcapReader
import matplotlib.pyplot as plt
import xlsxwriter as xw
import numpy as np
#新建excel
workbook  = xw.Workbook('04.xlsx')
#新建工作薄
worksheet = workbook.add_worksheet()

seqlist,seqlist2 = [],[]
acklist,acklist2 = [],[]
x,sl,x1=[],[],[]
name = '0.00002t11'
t = '2'
lb=['t','t10','t9']#,'t7','t5','t3','n']
tag=['','-1']
row,col,j,start=0,0,0,0
end=60*1000000
plt.xlabel("Time/s") 
plt.ylabel("rate") 
a1,a2,di,t1,t2,num=0,0,[],0,0,0

for root,dir,f in os.walk(name):
    for file in f:
        packets = scapy.rdpcap(name+'/'+file)
        dic={}
        a1,a2,di,t1,t2,num=0,0,[],0,0,0
        for packet in packets:
            if(float(packet.time)>60):
                break
            if(packet['TCP'].sport==80):
                num+=1
                tm = float(packet.time)
                a2=tm-t1
                di.append(abs(a2-a1))
                t1=tm
                a1=a2
                if(abs(2*tm-round(2*tm))<=0.02 ): #and roundtm/
                    dic[round(tm/0.5)]=sum(di[2:])/(num-2)
        x.append(dic)

np.save(name+"jt.npy",x)
values=[]
for i in range(1,121):
    ans,num=0,0
    for item in x:
        if(item.get(i)):
            ans+=item.get(i)
            num+=1
    values.append(ans/num)    

np.save(name+"jtp.npy",values)
plt.tight_layout()
plt.legend()
