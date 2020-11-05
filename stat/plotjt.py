import numpy as np
import matplotlib.pyplot as plt
x=[]
i=0
font={
    'weight':'semibold',
      'size':14
}
nm=['njt','t5jt','t7jt','t9jt','t11jt'] # ,'t10','t11']
ns=list(np.load("0.0000p.npy"))[::2] #avg rate when error = 0
nsl=list(np.load("0.0000l.npy"))[::2]  #low rate
nsh=list(np.load("0.0000h.npy"))[::2]  #high rate
for a in nm:  #iterate filename
    x.append([])
    for b in ['p','l','h']:
        x[i].append(list(np.load("0.00003"+a+b+".npy"))[::2])
    i=i+1


rg=np.arange(0.5,60.5,1)
plt.xlabel("Time (s)",fontdict=font) 
plt.ylabel("Jitter (s)",fontdict=font) 
#plt.plot(x2,acklist,label="No Spoofing") 
# plt.hlines(60,0.5,60) #horizontal line
# plt.hlines(70,0.5,60) 
# plt.hlines(80,0.5,60) 
# plt.hlines(90,0.5,60) 
# plt.hlines(100,0.5,60) 
# plt.hlines(110,0.5,60) 
j=0
lb=''
pl=['v','v','o','o','s','s']
cl=['none','g','none','orange','none','c']
cl1=['b','g','r','orange','k','c','m']
for b in nm:
    if b=='njt':
        lb='No Spoofing'
    else:
        lb='T='+b[1:][:-2]+"0KBps"
    plt.scatter(rg[4:], np.array(x[j][0])[4:], s=22,marker=pl[j],label=lb,facecolors=cl[j], edgecolors=cl1[j])#ms=4.5,
    plt.fill_between(rg[4:], np.array(x[j][1])[4:], np.array(x[j][2])[4:], alpha=0.2,color=cl1[j])
    plt.plot(rg[4:],np.array(x[j][0])[4:],linewidth=0.5,color=cl1[j])#,color='g')   
    j+=1
# plt.plot(rg,np.array(ns)/1000,linewidth=1) 
# plt.plot(rg,np.array(ns)/1000,'s',ms=3,label="Error=0")#,fontdict=font) 
# plt.fill_between(rg, np.array(nsl)/1000, np.array(nsh)/1000, alpha=0.2)

#figure, ax = plt.subplots()
plt.tick_params(labelsize=15)
#plt.yticks(np.arange(0,120,10))
#labels = ax.get_xticklabels() + ax.get_yticklabels()
#[label.set_fontname('Times New Roman') for label in labels]
plt.grid(ls='--')
plt.title("PER=0.02",fontdict={'weight':'semibold','size':'16'})
plt.tight_layout()
plt.legend(prop=font)
plt.savefig('0.00003jt.png',dpi=1000)